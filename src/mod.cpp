#include <algorithm>
#include <cstdint>
#include <vector>
#include "openrac/mod.hpp"
#include "logic.hpp"

namespace {
openrac::Mod* g=nullptr;
openrac::UiPage page;
std::vector<int> visited;
std::uint16_t previous=0;
bool open_requested=false;
int pending_level=-1;
}

void load(openrac::Mod& mod){
    if(mod.game()!="rac1" || !mod.has_play()) throw openrac::Error("Fast Travel requires RAC1 play API");
    g=&mod;

    page=mod.ui_page([&mod](const openrac::UiPage& p,const openrac::UiEvent& event){
        const auto status=mod.play_status();
        openrac::UiContent c;
        c.title="Fast Travel";
        if(visited.empty()){
            c.items={openrac::UiItem::text("No visited planets yet")};
        }else{
            for(int level:visited){
                c.items.push_back(openrac::UiItem::button(
                    mod.level_name(level)+(level==status.level ? " (current)" : ""),
                    level!=status.level
                ));
            }
        }
        c.footer={"Only planets visited this session are listed","L2 + R2 + Cross opens this page"};
        p.set(c);

        if(event.is(OPENRAC_UI_CHOOSE)){
            const int index=event.item();
            if(index>=0 && index<static_cast<int>(visited.size()) && visited[index]!=status.level){
                pending_level=visited[index];
                p.back();
            }
        }
    });

    mod.on_pad([](int port,std::uint8_t* report,int size){
        if(port!=0 || report==nullptr || size<2) return;
        const auto now=qol::pad_buttons(report);
        const auto mask=static_cast<std::uint16_t>(OPENRAC_PAD_L2|OPENRAC_PAD_R2|OPENRAC_PAD_CROSS);
        if(qol::combo_pressed(now,previous,mask)) open_requested=true;
        if((now&mask)==mask) qol::consume_buttons(report,mask);
        previous=now;
    });

    mod.on_frame([]{
        const auto status=g->play_status();
        if(status.level>=0 && std::find(visited.begin(),visited.end(),status.level)==visited.end())
            visited.push_back(status.level);

        if(open_requested){
            open_requested=false;
            page.open();
        }
        if(pending_level>=0 && status.phase==OPENRAC_PLAY_PLAYING){
            const int level=pending_level;
            pending_level=-1;
            g->play_travel(level);
        }
    });
}
OPENRAC_MOD_CPP("fast-travel","0.2.0",load)
