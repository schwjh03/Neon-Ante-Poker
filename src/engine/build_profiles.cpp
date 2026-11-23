#include "build_profiles.hpp"
#include <algorithm>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <numeric>
#include <set>
#include <sstream>

namespace neon {
bool BuildProfile::valid() const {
    if(mode<0||mode>3||ante<1||ante>10000||round<1||round>30000||owned.size()>5||best<0||best>9000000000000000LL||lifetime<0||lifetime>9000000000000000000LL)return false;
    std::set<int> ids;for(int id:owned)if(id<0||id>=16||!ids.insert(id).second)return false;
    for(int n:levels)if(n<1||n>100000)return false;
    for(int n:handUse)if(n<0||n>1000000)return false;
    for(const Card& c:deck)if(c.rank<2||c.rank>14||c.suit<0||c.suit>3)return false;
    return true;
}
int BuildProfile::favoredHand() const {
    auto bestUse=std::max_element(handUse.begin(),handUse.end());
    return *bestUse>0?static_cast<int>(bestUse-handUse.begin()):-1;
}
double BuildProfile::strength(size_t historyCount) const {
    int upgrades=0;for(size_t i=0;i<deck.size();++i){const auto& c=deck[i];upgrades+=c.foil+c.wild+std::max(0,c.rank-(static_cast<int>(i)/4+2));}
    double relicPower=0;for(size_t i=0;i<owned.size();++i){const int id=owned[i];relicPower+=(id==2||id==9||id==10||id==12)?.018+i*.007:.008;}
    int levelsGained=0;for(int n:levels)levelsGained+=std::min(20,n-1);
    return std::clamp(1.+std::min(ante,16)*.025+std::min(upgrades,60)*.003+std::min(levelsGained,40)*.006+std::min<size_t>(historyCount,8)*.015+relicPower,1.05,1.8);
}
std::string BuildProfile::summary() const {
    const int hand=favoredHand();return "Ante "+std::to_string(ante)+" / "+(hand<0?"Uncommitted":handNames[hand])+" / "+std::to_string(owned.size())+" relics";
}
BuildProfile captureBuild(const Game& g){return {g.runSeed,static_cast<int>(g.mode),g.ante,g.round,g.best,g.lifetime,g.levels,g.handUse,g.deck,g.owned};}
static void writeProfile(std::ostream& out,const BuildProfile& p){
    out<<p.seed<<' '<<p.mode<<' '<<p.ante<<' '<<p.round<<' '<<p.best<<' '<<p.lifetime<<'\n';
    for(int n:p.levels){out<<n<<' ';}
    out<<'\n';
    for(int n:p.handUse){out<<n<<' ';}
    out<<'\n';
    out<<p.owned.size();for(int id:p.owned)out<<' '<<id;out<<'\n';
    for(auto c:p.deck)out<<c.rank<<' '<<c.suit<<' '<<c.foil<<' '<<c.wild<<'\n';
}
static bool readProfile(std::istream& in,BuildProfile& p){
    if(!(in>>p.seed>>p.mode>>p.ante>>p.round>>p.best>>p.lifetime))return false;
    for(int& n:p.levels){if(!(in>>n))return false;}
    for(int& n:p.handUse){if(!(in>>n))return false;}
    size_t count=0;if(!(in>>count)||count>5)return false;p.owned.resize(count);
    for(int& id:p.owned)if(!(in>>id))return false;
    for(auto& c:p.deck)if(!(in>>c.rank>>c.suit>>c.foil>>c.wild))return false;
    return p.valid();
}
bool exportBuild(const BuildProfile& profile,const std::filesystem::path& path){
    if(!profile.valid())return false;
    std::ofstream out(path);if(!out)return false;
    out<<"NEON_ANTE_BUILD 1\n";writeProfile(out,profile);out.close();return static_cast<bool>(out);
}
bool importBuild(const std::filesystem::path& path,BuildProfile& profile){
    std::error_code ec;const auto bytes=std::filesystem::file_size(path,ec);if(ec||bytes>8192)return false;
    std::ifstream in(path);std::string magic;int version=0;BuildProfile candidate;
    if(!(in>>magic>>version)||magic!="NEON_ANTE_BUILD"||version!=1||!readProfile(in,candidate))return false;
    in>>std::ws;if(!in.eof())return false;profile=std::move(candidate);return true;
}
bool BuildLibrary::add(const BuildProfile& profile){
    if(!profile.valid())return false;
    for(const auto& p:profiles)if(p.seed==profile.seed&&p.mode==profile.mode&&p.round==profile.round&&p.lifetime==profile.lifetime&&p.owned==profile.owned&&p.levels==profile.levels&&p.handUse==profile.handUse&&std::equal(p.deck.begin(),p.deck.end(),profile.deck.begin(),[](auto a,auto b){return a.rank==b.rank&&a.suit==b.suit&&a.foil==b.foil&&a.wild==b.wild;}))return false;
    profiles.push_back(profile);if(profiles.size()>8){const auto weakest=std::min_element(profiles.begin(),profiles.end(),[](const auto& a,const auto& b){return a.strength(8)<b.strength(8);});profiles.erase(weakest);}return true;
}
const BuildProfile* BuildLibrary::strongest() const {
    if(profiles.empty())return nullptr;
    return &*std::max_element(profiles.begin(),profiles.end(),[&](const auto& a,const auto& b){return a.strength(profiles.size())<b.strength(profiles.size());});
}
bool BuildLibrary::save(const std::filesystem::path& path) const {
    if(profiles.size()>8)return false;
    for(const auto& p:profiles){if(!p.valid())return false;}
    auto temp=path;temp+=".tmp";std::ofstream out(temp);if(!out)return false;
    out<<"NEON_ANTE_LIBRARY 1 "<<profiles.size()<<'\n';for(const auto& p:profiles)writeProfile(out,p);out.close();if(!out)return false;
    std::error_code ec;std::filesystem::copy_file(temp,path,std::filesystem::copy_options::overwrite_existing,ec);if(ec)return false;std::filesystem::remove(temp,ec);return true;
}
bool BuildLibrary::load(const std::filesystem::path& path){
    std::error_code ec;const auto bytes=std::filesystem::file_size(path,ec);if(ec||bytes>65536)return false;
    std::ifstream in(path);std::string magic;int version=0;size_t count=0;
    if(!(in>>magic>>version>>count)||magic!="NEON_ANTE_LIBRARY"||version!=1||count>8)return false;
    std::vector<BuildProfile> next(count);for(auto& p:next)if(!readProfile(in,p))return false;
    in>>std::ws;if(!in.eof())return false;profiles=std::move(next);return true;
}
uint32_t dailySeedFor(const std::string& day){uint32_t value=2166136261u;for(unsigned char c:day){value^=c;value*=16777619u;}return value;}
std::string utcDay(){const auto stamp=std::time(nullptr);const auto time=std::gmtime(&stamp);if(!time)return "unknown";std::ostringstream out;out<<std::put_time(time,"%Y-%m-%d");return out.str();}
} // namespace neon
