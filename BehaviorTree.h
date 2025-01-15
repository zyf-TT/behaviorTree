#include <stdint.h>

typedef enum
{
    NODE_TYPE_ACTION,
    NODE_TYPE_CONDITION,
    NODE_TYPE_SEQUENCE,
    NODE_TYPE_SELECTOR,
    NODE_TYPE_PARALLEL,
    NODE_TYPE_DECORATOR,
    NODE_TYPE_MEMORY
} NodeType;

typedef enum
{
    DECORATOR_TYPE_INVERT,
    DECORATOR_TYPE_REPEAT,
    DECORATOR_TYPE_REPEAT_UNTIL_SUCCESS,
    DECORATOR_TYPE_CONDITIONAL,
    DECORATOR_TYPE_DELAY
} DecoratorType;

typedef union
{
    uint32_t delay;
    uint32_t repeat;
} DecoratorParams;

typedef struct Decorator
{
    DecoratorType type;
    DecoratorParams params;
    int reference_count; // 引用计数
} Decorator;

typedef struct BehaviorNode
{
    Decorator *decorator;
    int (*action)(void); // Action function pointer
    int child_count;
    int reference_count; // 引用计数
    NodeType type;
    struct BehaviorNode **children;
} BehaviorNode;

// Function prototypes
int executeNode(BehaviorNode *node);
BehaviorNode *createBehaviorNode(BehaviorNode **children,
                                 int child_count,
                                 NodeType type,
                                 int (*actionFunc)(void));
Decorator *createEmptyDecorator();
Decorator *createRepeatDecorator(uint32_t repeatCount);
Decorator *createDelayDecorator(uint32_t delayTime);
Decorator *createConditionalDecorator();
Decorator *createDecorator(DecoratorType type,
                           void *param);
int freeBehaviorTree(BehaviorNode *node);