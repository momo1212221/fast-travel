#include <algorithm>
#include <cstdint>
#include <string>
#include <vector>
#include "openrac/mod.hpp"
#include "logic.hpp"

namespace {
openrac::Mod* g=nullptr;
std::vector<int> visited;
std::size_t selected=0;
std::uint16_t previous=0;
int action=0;
void say(){ if(!visited.empty() && g->has_notice()) g->play_notice("Fast travel: "+g->level_name(visited[selected])); }
}
void load(openrac::Mod& mod){
    if(mod.game()!="rac1" || !mod.has_play()) throw openrac::Error("Fast Travel requires RAC1 play API");
    g=&mod;
    mod.on_pad([](int port,std::uint8_t* report,int size){
        if(port!=0 || report==nullptr || size<2) return;
        const auto now=qol::pad_buttons(report);
        const auto left=static_cast<std::uint16_t>(OPENRAC_PAD_L2|OPENRAC_PAD_R2|OPENRAC_PAD_LEFT);
        const auto right=static_cast<std::uint16_t>(OPENRAC_PAD_L2|OPENRAC_PAD_R2|OPENRAC_PAD_RIGHT);
        const auto go=static_cast<std::uint16_t>(OPENRAC_PAD_L2|OPENRAC_PAD_R2|OPENRAC_PAD_CROSS);
        if(qol::combo_pressed(now,previous,left)) action=-1;
        if(qol::combo_pressed(now,previous,right)) action=1;
        if(qol::combo_pressed(now,previous,go)) action=2;
        if((now&left)==left) qol::consume_buttons(report,left);
        if((now&right)==right) qol::consume_buttons(report,right);
        if((now&go)==go) qol::consume_buttons(report,go);
        previous=now;
    });
    mod.on_frame([]{
        const auto s=g->play_status();
        if(s.level>=0 && std::find(visited.begin(),visited.end(),s.level)==visited.end()){ visited.push_back(s.level); selected=visited.size()-1; }
        if(action==-1 || action==1){ selected=qol::step(selected,visited.size(),action); action=0; say(); }
        else if(action==2){ action=0; if(!visited.empty() && s.phase==OPENRAC_PLAY_PLAYING){ if(visited[selected]==s.level) say(); else g->play_travel(visited[selected]); } }
    });
    mod.info("L2+R2+Left/Right selects; L2+R2+Cross travels");
}
OPENRAC_MOD_CPP("fast-travel","0.1.1",load)
