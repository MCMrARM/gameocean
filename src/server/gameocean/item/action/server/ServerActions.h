#pragma once

class UseItemAction;

class ServerActions {

protected:

    static bool openContainer(UseItemAction& action);

public:

    static void registerActions();

};


