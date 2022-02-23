
#include "../../include/client/notificationmanagerclient.hpp"
#include <unistd.h>

void NotificationManager::listen(){
    // TODO
    // 1. Listen for notifications
    // 2. if is a valid notification use interface.updateNotifications(notification)

    // TEST MESSAGE
    while (true){
        usleep(1000000);
        interface.updateNotifications("FOI - user");
    }
}