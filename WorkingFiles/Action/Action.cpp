//
// Created by Eric Thomas on 9/14/23.
//

#include "Action.h"

Action Action::generate_none_action(int iAgentID) {
    Action action;
    action.iAgentID = iAgentID;
    action.enumActionType = ActionType::enumNoneAction;
    return action;
};
