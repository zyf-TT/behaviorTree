#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "BehaviorTree.h"
// Action functions
int actionA()
{
    printf("Action A executed\n");
    return 1;
}

int actionB()
{
    printf("Action B executed\n");
    return 1;
}

int actionFail()
{
    printf("Action Failed\n");
    return 0;
}

int beep()
{
    printf("Beep is start\n");
}

int motor()
{
    printf("Motor is start\n");
}

// Example condition function
int checkCondition()
{
    static int count = 0;
    printf("Condition checked %d \n", count);
    if (count++ > 3)
    {
        return 1;
    }
    return 0; // Simulate condition is true
}

int ifCondition()
{
    printf("ifCondition \n");
    return 1;
}

int if_test()
{
    printf("if_test \n");
    return 1;
}

int else_test()
{
    printf("Else test\n");
    return 0;
}
int main()
{
    int consult = 0;
    // Create action nodes
    BehaviorNode *action1 = createBehaviorNode(NULL,
                                               0,
                                               NODE_TYPE_ACTION,
                                               actionA);
    BehaviorNode *action2 = createBehaviorNode(NULL,
                                               0,
                                               NODE_TYPE_ACTION,
                                               actionB);

    BehaviorNode *Beep = createBehaviorNode(NULL,
                                            0,
                                            NODE_TYPE_ACTION,
                                            beep);
    BehaviorNode *Motor = createBehaviorNode(NULL,
                                             0,
                                             NODE_TYPE_ACTION,
                                             motor);
    BehaviorNode *conditionNode = createBehaviorNode(NULL,
                                                     0,
                                                     NODE_TYPE_CONDITION,
                                                     checkCondition);
    // Create a sequence node
    BehaviorNode *sequenceChildren[] = {Motor, Beep};
    BehaviorNode *sequence = createBehaviorNode(sequenceChildren,
                                                2,
                                                NODE_TYPE_SEQUENCE,
                                                NULL);

    // Create a decorator node
    BehaviorNode *delayChildren[] = {sequence};
    BehaviorNode *delay_1s = createBehaviorNode(delayChildren,
                                                1,
                                                NODE_TYPE_DECORATOR,
                                                NULL);
    delay_1s->decorator = createDelayDecorator(1);
    // Create a decorator node
    BehaviorNode *repeatChildren[] = {delay_1s};
    BehaviorNode *repeat = createBehaviorNode(repeatChildren,
                                              1,
                                              NODE_TYPE_DECORATOR,
                                              NULL);
    repeat->decorator = createRepeatDecorator(3);
    //
    BehaviorNode *delay_10sChildren[] = {action1};
    BehaviorNode *delay_10s = createBehaviorNode(delay_10sChildren,
                                                 1,
                                                 NODE_TYPE_DECORATOR,
                                                 NULL);
    delay_10s->decorator = createDelayDecorator(3);
    // Create a selector node
    BehaviorNode *selectorChildren[] = {delay_10s, action2};
    BehaviorNode *selector = createBehaviorNode(selectorChildren,
                                                2,
                                                NODE_TYPE_SELECTOR,
                                                NULL);

    // Create a conditional decorator node
    BehaviorNode *ifConditionNode = createBehaviorNode(NULL,
                                                       0,
                                                       NODE_TYPE_ACTION,
                                                       ifCondition);
    BehaviorNode *if_testNode = createBehaviorNode(NULL,
                                                   0,
                                                   NODE_TYPE_ACTION,
                                                   if_test);
    BehaviorNode *else_testNode = createBehaviorNode(NULL,
                                                     0,
                                                     NODE_TYPE_ACTION,
                                                     else_test);
    BehaviorNode *ifteseChilds[] = {ifConditionNode, if_testNode, else_testNode};
    BehaviorNode *ifTestNode = createBehaviorNode(ifteseChilds,
                                                  2,
                                                  NODE_TYPE_DECORATOR,
                                                  NULL);
    ifTestNode->decorator = createConditionalDecorator();
    // Create a parallel node
    BehaviorNode *parallelChildren[] = {ifTestNode, selector, action1, repeat};
    // BehaviorNode *parallelChildren[] = {action1};
    BehaviorNode *root = createBehaviorNode(parallelChildren,
                                            4,
                                            NODE_TYPE_PARALLEL,
                                            NULL);

    executeNode(root);
    printf("--\n");
    // Free memory
    freeBehaviorTree(root);
    printf("freeBehaviorTree(root)\n");
    return 0;
}