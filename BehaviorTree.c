#include "BehaviorTree.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static int invertDecorator(BehaviorNode *node);
static int repeatDecorator(BehaviorNode *node);
static int conditionalDecorator(BehaviorNode *node);
static int repeatUntilSuccessDecorator(BehaviorNode *node);
static int delayDecorator(BehaviorNode *node);
static int sequenceNode(BehaviorNode *node);
static int selectorNode(BehaviorNode *node);
static int decoratorNode(BehaviorNode *node);
static int parallelNode(BehaviorNode *node);
static BehaviorNode *BehaviorNodeCheck(NodeType type, BehaviorNode *node);
static BehaviorNode *checkActionNode(BehaviorNode *node);
static BehaviorNode *checkConditionNode(BehaviorNode *node);
static BehaviorNode *checkSequenceNode(BehaviorNode *node);
static BehaviorNode *checkSelectorNode(BehaviorNode *node);
static BehaviorNode *checkParallelNode(BehaviorNode *node);
static BehaviorNode *checkDecoratorNode(BehaviorNode *node);
static BehaviorNode *checkMemoryNode(BehaviorNode *node);
static void handleMemoryError();

/**
 * @brief Executes a node in the behavior tree.
 *
 * This function executes the given behavior node based on its type. It handles
 * different node types including action, condition, sequence, selector, decorator,
 * and parallel nodes. If the node is NULL, it treats it as a failure.
 *
 * @param node Pointer to the BehaviorNode to be executed.
 * @return int Returns 1 if the node execution succeeds, 0 if it fails or for unknown node types.
 */
int executeNode(BehaviorNode *node)
{
    if (node == NULL)
    {
        handleMemoryError();
        return 0; // Treat NULL as failure
    }

    switch (node->type)
    {
    case NODE_TYPE_ACTION:
        return node->action();
    case NODE_TYPE_CONDITION:
        return node->action();
    case NODE_TYPE_SEQUENCE:
        // Recursive call to handle sequence nodes
        return sequenceNode(node);
    case NODE_TYPE_SELECTOR:
        // Recursive call to handle selector nodes
        return selectorNode(node);
    case NODE_TYPE_DECORATOR:
        // Recursive call to handle decorator nodes
        return decoratorNode(node);
    case NODE_TYPE_PARALLEL:
        // Recursive call to handle parallel nodes
        return parallelNode(node);
    default:
        return 0; // Unknown node type
    }
}

// Sequence node behavior
/**
 * @brief Executes a sequence node in the behavior tree.
 *
 * This function iterates over all child nodes of a sequence node and executes them
 * in order. The sequence node succeeds only if all child nodes succeed.
 *
 * @param node Pointer to the BehaviorNode structure representing the sequence node.
 *             It must have a valid list of child nodes to execute.
 * @return int Returns 1 if all child nodes succeed, 0 if any child node fails.
 */
static int sequenceNode(BehaviorNode *node)
{
    for (int i = 0; i < node->child_count; i++)
    {
        int result = executeNode(node->children[i]);

        if (!result)
        {
            return 0; // Failure
        }
    }
    return 1; // Success
}

/**
 * @brief Executes a selector node in the behavior tree.
 *
 * This function iterates over all child nodes of a selector node and executes them
 * in order. The selector node succeeds if any child node succeeds.
 *
 * @param node Pointer to the BehaviorNode structure representing the selector node.
 *             It must have a valid list of child nodes to execute.
 * @return int Returns 1 if any child node succeeds, 0 if all child nodes fail.
 */
static int selectorNode(BehaviorNode *node)
{
    for (int i = 0; i < node->child_count; i++)
    {
        int result = executeNode(node->children[i]);

        if (result)
        {
            return 1; // If any child succeeds, the selector succeeds
        }
    }
    return 0; // Failure if all children fail
}

/**
 * @brief Executes a decorator node in the behavior tree.
 *
 * This function processes a decorator node by executing its child node
 * according to the decorator's type. It supports various decorator types
 * such as invert, repeat, repeat until success, conditional, and delay.
 *
 * @param node Pointer to the BehaviorNode structure representing the decorator node.
 *             It must have exactly one child node and a valid decorator.
 * @return int Returns the result of the decorator's execution:
 *             - 1 for success
 *             - 0 for failure or if the decorator type is unknown
 */
static int decoratorNode(BehaviorNode *node)
{
    Decorator *decorator = node->decorator;

    if (node->child_count == 0)
    {
        handleMemoryError();
        return 0; // 可以考虑失败返回值
    }
    switch (decorator->type)
    {
    case DECORATOR_TYPE_INVERT:
        return invertDecorator(node);

    case DECORATOR_TYPE_REPEAT:
        return repeatDecorator(node);

    case DECORATOR_TYPE_REPEAT_UNTIL_SUCCESS:
        return repeatUntilSuccessDecorator(node);

    case DECORATOR_TYPE_CONDITIONAL:
        return conditionalDecorator(node);

    case DECORATOR_TYPE_DELAY:
        return delayDecorator(node);

    default:
        return 0;
    }
}

/**
 * @brief Executes an invert decorator node in the behavior tree.
 *
 * This function processes an invert decorator node by executing its child node
 * and inverting the result. If the child node succeeds, the invert decorator
 * returns failure, and vice versa.
 *
 * @param node Pointer to the BehaviorNode structure representing the invert decorator node.
 *             It must have exactly one child node.
 * @return int Returns 1 if the child node fails, 0 if the child node succeeds.
 */
static int invertDecorator(BehaviorNode *node)
{
    return executeNode(node->children[0]) == 0 ? 1 : 0; // 反转结果
}

/**
 * @brief Executes a repeat decorator node in the behavior tree.
 *
 * This function processes a repeat decorator node by executing its child node
 * a specified number of times. The repeat count is determined by the decorator's
 * parameters. The function returns the result of the last execution of the child node.
 *
 * @param node Pointer to the BehaviorNode structure representing the repeat decorator node.
 *             It must have exactly one child node and a valid repeat decorator.
 * @return int Returns the result of the last execution of the child node:
 *             - 1 for success
 *             - 0 for failure
 */
static int repeatDecorator(BehaviorNode *node)
{
    Decorator *decorator = node->decorator;
    uint32_t repeat = decorator->params.repeat;

    int result;
    while (repeat--)
    {
        result = executeNode(node->children[0]);
    }
    return result; // 返回最后的执行结果
}

static int repeatUntilSuccessDecorator(BehaviorNode *node)
{
    Decorator *decorator = node->decorator;
    int result;

    do
    {
        result = executeNode(node->children[0]);
    } while (result);

    return result; // 返回最后的执行结果
}

static int conditionalDecorator(BehaviorNode *node)
{
    Decorator *decorator = node->decorator;
    int result;
    result = executeNode(node->children[0]);

    if (node->child_count == 1)
        return result;

    if (result)
    {
        return executeNode(node->children[1]);
    }
    if (node->child_count < 3)
    {
        return 0;
    }
    return executeNode(node->children[2]);
}

static int delayDecorator(BehaviorNode *node)
{
    Decorator *decorator = node->decorator;
    uint32_t delay = decorator->params.delay;

    int result = executeNode(node->children[0]);
    if (result == 1)
    {
        sleep(delay);
    }
    return result;
}

// Parallel node behavior (example: succeeds if all children succeed)
/**
 * @brief Executes a parallel node in the behavior tree.
 *
 * This function executes all child nodes of a parallel node concurrently.
 * It counts the number of successful executions and considers the parallel
 * node successful only if all child nodes succeed.
 *
 * @param node Pointer to the BehaviorNode structure representing the parallel node.
 * @return int Returns 1 if all child nodes succeed, 0 otherwise.
 */
static int parallelNode(BehaviorNode *node)
{
    int successCount = 0;
    for (int i = 0; i < node->child_count; i++)
    {
        int result = executeNode(node->children[i]);
        if (!result)
        {
            successCount++;
        }
    }
    return (successCount == node->child_count) ? 1 : 0; // Succeeds if all succeed
}

BehaviorNode *createBehaviorNode(BehaviorNode **children,
                                 int child_count,
                                 NodeType type,
                                 int (*actionFunc)(void))
{
    // 分配节点内存
    BehaviorNode *node = (BehaviorNode *)malloc(sizeof(BehaviorNode));
    if (!node)
    {
        handleMemoryError();
        return NULL;
    }
    printf("create behavior node, the type is:%d \n", type);
    // 初始化节点
    node->type = type;
    node->action = actionFunc;
    node->reference_count = 0; // 初始引用计数设置为 1
    node->child_count = child_count;

    // 分配子节点指针的内存
    if (child_count > 0)
    {
        node->children = (BehaviorNode **)malloc(sizeof(BehaviorNode *) * child_count);
        if (!node->children)
        {
            free(node); // 释放已分配的节点内存
            handleMemoryError();
            return NULL;
        }

        for (int i = 0; i < child_count; i++)
        {
            node->children[i] = children[i]; // 指向子节点
            if (children[i])
            {
                children[i]->reference_count++; // 增加子节点的引用计数
            }
        }
    }
    else
    {
        node->children = NULL; // No children
    }

    return BehaviorNodeCheck(type, node);
}

Decorator *createEmptyDecorator()
{
    Decorator *decorator = (Decorator *)malloc(sizeof(Decorator));
    if (!decorator)
    {
        handleMemoryError();
        return NULL;
    }
    // 初始化结构体
    memset(decorator, 0, sizeof(Decorator));
    return decorator;
}

Decorator *createRepeatDecorator(uint32_t repeatCount)
{
    Decorator *decorator = createEmptyDecorator();
    decorator->type = DECORATOR_TYPE_REPEAT;
    decorator->params.repeat = repeatCount;
    return decorator;
}

Decorator *createDelayDecorator(uint32_t delayTime)
{
    Decorator *decorator = createEmptyDecorator();
    decorator->type = DECORATOR_TYPE_DELAY;
    decorator->params.repeat = delayTime; // 这里可以使用同一个参数
    return decorator;
}

Decorator *createConditionalDecorator()
{
    Decorator *decorator = createEmptyDecorator();
    if (!decorator)
    {
        handleMemoryError();
        return NULL;
    }
    decorator->type = DECORATOR_TYPE_CONDITIONAL;
    return decorator;
}

Decorator *createDecorator(DecoratorType type, void *param)
{
    Decorator *decorator = createEmptyDecorator();
    if (!decorator)
    {
        handleMemoryError();
        return NULL;
    }

    decorator->type = type;

    switch (type)
    {
    case DECORATOR_TYPE_REPEAT:
    case DECORATOR_TYPE_REPEAT_UNTIL_SUCCESS:
    case DECORATOR_TYPE_DELAY:
        decorator->params.repeat = *(uint32_t *)param;
        break;
    case DECORATOR_TYPE_CONDITIONAL:
        break;
    case DECORATOR_TYPE_INVERT:
        // 其它特定初始化
        break;
    default:
        // 处理未知的类型（可选）
        break;
    }
    return decorator;
}

static BehaviorNode *BehaviorNodeCheck(NodeType type, BehaviorNode *node)
{
    switch (type)
    {
    case NODE_TYPE_ACTION:
        return checkActionNode(node);
    case NODE_TYPE_CONDITION:
        return checkConditionNode(node);
    case NODE_TYPE_SEQUENCE:
        return checkSequenceNode(node);
    case NODE_TYPE_SELECTOR:
        return checkSelectorNode(node);
    case NODE_TYPE_PARALLEL:
        return checkParallelNode(node);
    case NODE_TYPE_DECORATOR:
        return checkDecoratorNode(node);
    case NODE_TYPE_MEMORY:
        return checkMemoryNode(node);
    default:
        return NULL;
    }
}

static BehaviorNode *checkActionNode(BehaviorNode *node)
{
    if (node == NULL)
        return NULL;

    if (node->action == NULL)
        return NULL;

    if (node->children != NULL)
        return NULL;

    if (node->child_count != 0)
        return NULL;

    return node;
}

static BehaviorNode *checkConditionNode(BehaviorNode *node)
{
    if (node == NULL)
        return NULL;

    if (node->action == NULL)
        return NULL;

    if (node->children != NULL)
        return NULL;

    if (node->child_count != 0)
        return NULL;

    return node;
}

/**
 * @brief Validates the structure of a sequence node in the behavior tree.
 *
 * This function checks if the given node meets the criteria for a valid sequence node.
 * A valid sequence node must have at least one child, no decorator, and no action function.
 *
 * @param node Pointer to the BehaviorNode structure to be checked.
 *             This node is expected to be of type NODE_TYPE_SEQUENCE.
 *
 * @return BehaviorNode* Returns the input node if it's a valid sequence node,
 *                       NULL otherwise (indicating an invalid sequence node structure).
 */
static BehaviorNode *checkSequenceNode(BehaviorNode *node)
{
    if (node == NULL)
        return NULL;

    if (node->children == NULL || node->decorator != NULL)
        return NULL;

    if (node->child_count == 0)
        return NULL;

    if (node->action != NULL)
        return NULL;

    return node;
}

/**
 * @brief Validates the structure of a selector node in the behavior tree.
 *
 * This function checks if the given node meets the criteria for a valid selector node.
 * A valid selector node must have at least one child, no decorator, and no action function.
 *
 * @param node Pointer to the BehaviorNode structure to be checked.
 *             This node is expected to be of type NODE_TYPE_SELECTOR.
 *
 * @return BehaviorNode* Returns the input node if it's a valid selector node,
 *                       NULL otherwise (indicating an invalid selector node structure).
 */
static BehaviorNode *checkSelectorNode(BehaviorNode *node)
{
    if (node == NULL)
        return NULL;

    if (node->children == NULL || node->decorator != NULL)
        return NULL;

    if (node->child_count == 0)
        return NULL;

    if (node->action != NULL)
        return NULL;
    return node;
}

/**
 * @brief Validates the structure of a parallel node in the behavior tree.
 *
 * This function checks if the given node meets the criteria for a valid parallel node.
 * A valid parallel node must have exactly one child, no decorator, and no action function.
 *
 * @param node Pointer to the BehaviorNode structure to be checked.
 *             This node is expected to be of type NODE_TYPE_PARALLEL.
 *
 * @return BehaviorNode* Returns the input node if it's a valid parallel node,
 *                       NULL otherwise (indicating an invalid parallel node structure).
 */
static BehaviorNode *checkParallelNode(BehaviorNode *node)
{
    if (node == NULL)
        return NULL;

    if (node->children == NULL || node->decorator != NULL)
        return NULL;

    if (node->child_count == 0)
        return NULL;

    if (node->action != NULL)
        return NULL;

    return node;
}
/**
 * @brief Validates the structure of a memory node in the behavior tree.
 *
 * This function is intended to check if the given node meets the criteria for a valid memory node.
 * However, the current implementation always returns NULL, indicating that memory nodes are not
 * yet supported or implemented in this behavior tree system.
 *
 * @param node Pointer to the BehaviorNode structure to be checked.
 *             This node is expected to be of type NODE_TYPE_MEMORY.
 *
 * @return BehaviorNode* Always returns NULL in the current implementation, indicating
 *                       that memory nodes are not supported or the check fails.
 */
static BehaviorNode *checkMemoryNode(BehaviorNode *node)
{
    return NULL;
}

/**
 * @brief Validates the structure of a decorator node in the behavior tree.
 *
 * This function checks if the given node meets the criteria for a valid decorator node.
 * A valid decorator node must have at least one and at most three children, and a non-null decorator.
 *
 * @param node Pointer to the BehaviorNode structure to be checked.
 *             This node is expected to be of type NODE_TYPE_DECORATOR.
 *
 * @return BehaviorNode* Returns the input node if it's a valid decorator node,
 *                       NULL otherwise (indicating an invalid decorator node structure).
 */
static BehaviorNode *checkDecoratorNode(BehaviorNode *node)
{
    if (node == NULL)
        return NULL;

    if (node->children == NULL)
        return NULL;

    if (node->child_count > 3 || node->child_count == 0)
        return NULL;

    return node;
}

// 释放行为树
/**
 * @brief Recursively frees the memory allocated for a behavior tree.
 *
 * This function traverses the behavior tree starting from the given node,
 * recursively freeing all child nodes, managing decorators, and finally
 * freeing the node itself. It uses reference counting to ensure proper
 * memory management for shared nodes and decorators.
 *
 * @param node Pointer to the root BehaviorNode of the tree or subtree to be freed.
 *             If NULL, the function does nothing.
 *
 * @return void This function does not return a value.
 */
int freeBehaviorTree(BehaviorNode *node)
{
    int result;
    if (!node)
    {
        return 0;
    }
    // 递归释放子节点
    for (int i = 0; i < node->child_count; i++)
    {
        if (node->children[i] == NULL)
        {
            return 0;
        }
        result = freeBehaviorTree(node->children[i]);
        if (result == 0)
        {
            return 0;
        }
        }

    // 管理 Decorator
    if (node->decorator) // 确保 decorator 不为 NULL
    {
        // 减少引用计数
        if (--node->decorator->reference_count == 0)
        {
            free(node->decorator);
            node->decorator = NULL;
        }
    }

    // 减少行为节点的引用计数并释放自身
    if (--node->reference_count == 0)
    {
        free(node);
        node = NULL;
    }
    return 1;
}

/**
 * @brief Handles memory allocation errors.
 *
 * This function is called when a memory allocation fails. It prints an error
 * message to the standard error stream and terminates the program.
 *
 * @note This function does not return as it calls exit().
 */
static void handleMemoryError()
{
    fprintf(stderr, "Memory allocation error for Decorator\n");
    exit(EXIT_FAILURE);
}