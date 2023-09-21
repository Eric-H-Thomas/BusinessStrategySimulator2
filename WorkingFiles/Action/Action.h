//
// Created by Eric Thomas on 9/14/23.
//

#pragma once
enum class ActionType {
    enumEntryAction,
    enumExitAction,
    enumNoneAction
};

class Action {
public:
    int iAgentID;
    ActionType enumActionType;
    int iMarketID;

    static Action generate_none_action(int iAgentID);
};
