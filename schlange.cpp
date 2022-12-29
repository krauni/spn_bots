// You can use the entire C/C++ standard library, just add the relevant
// #includes. We recommend math.h ;-)

#include "usercode.h"
#include "math.h"
#include <string>


ipc_guid_t nixId = -1;
bool isKiller = true;

ipc_guid_t find_snake(Api *api){
    for (size_t i = 0; i < api->getBotCount(); i++){
        const IpcBotInfo * botList = api->getBots();
        IpcBotInfo bot = botList[i];
        char* name = bot.bot_name;
        if (name[0] == 'K' &&name[1] == 'B'){
            api->log(std::to_string(nixId).c_str());
            return bot.bot_id;
        }
    }
    return -1;
}

bool init(Api *api)
{

    api->clearColors();
    api->addColor(0, 255, 33);
    

    nixId = find_snake(api);
    
    return true;
}

ipc_real_t  getMass(ipc_real_t  r){
    size_t rad_value = 3.98107 + r;
    return  (-100 + pow(rad_value,10/3)) / 2;
}

bool isRealDanger(ipc_real_t r, ipc_real_t ownMass){
    return getMass(r) > (0.001 * ownMass);
}


bool step(Api *api)
{
    const IpcSelfInfo* self_info = api->getSelfInfo();
    const IpcFoodInfo* food_list = api->getFood();

    //api->log(std::to_string(nixId).c_str());
    //size_t food_length = api->getFoodCount();
    ipc_real_t OTHER_SNEK_PANIC_DISTANCE = self_info->segment_radius *3;
    

    ipc_real_t OTHER_SNEK_PANIC_ANGLE  = 1.6;

    for(size_t i = 0; i < api->getSegmentCount(); i++) {
        const IpcSegmentInfo& seg = api->getSegments()[i];
        size_t safeDistance = OTHER_SNEK_PANIC_DISTANCE + seg.r*2.1;
        
        if(!seg.is_self && seg.dist  < safeDistance && fabs(seg.dir) < OTHER_SNEK_PANIC_ANGLE && isRealDanger(seg.r, self_info->mass)) {
            //api->log(std::to_string(safeDistance).c_str());
            if (isKiller && seg.bot_id == nixId && seg.idx == 0){
                api->boost = true;
                api->angle = seg.dir;
                api->log("found snake! Kamikazeeee");
                return true;
            }
            if(seg.dir < 0){
                api->angle = 1.7;
            } else {
                api->angle = -1.7;
            }
            if (seg.idx == 0 && seg.dist < self_info->segment_radius * 2 && fabs(seg.dir) > 1.57 ){
                api-> boost = true;
                api-> angle = seg.dir;
                api->log("Potential Killer behind me, running!");
            } else{
                api->boost =false;
            }
            if (seg.dist < self_info->segment_radius * 2 ){
                api-> boost = true;
                api->log("Panik!");
            } else {
                api-> boost = false;
            }
            //api->log("Ausweichen!");
            return true;
        }
    }

    ipc_real_t bestFoodValue = 0;
    size_t bestFoodIndex = 0;
    for(size_t i = 0;  i < api->getFoodCount(); i++) {
        if (food_list[i].dist > 1.1 * self_info->consume_radius && food_list[i].val > bestFoodValue && fabs(food_list[i].val) < 0.9){
            if (food_list[i].dist > self_info->consume_radius *3){
             break;
            }
            bestFoodValue = food_list[i].val;
            bestFoodIndex = i;
        }
    }
    float angle_to_next_food = food_list[bestFoodIndex].dir;
    
    api->angle = angle_to_next_food;


    return true;
}


