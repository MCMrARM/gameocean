#pragma once

class UseItemAction;
class ActionHandlerData;

class ServerActions {

protected:

    static bool openContainer(UseItemAction &action, ActionHandlerData *);

public:

    static void registerActions();

};


