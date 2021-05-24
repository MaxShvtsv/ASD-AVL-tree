#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DELETION 1
#define INSERTION 0

#define WORD_LEN 100

#define LEFT 1
#define RIGHT 0
#define NO_SIDE -1

typedef struct Word_
{
    char word[WORD_LEN];
    int chars_count;
} Word_;

typedef struct Node_t
{
    struct Node_t* left;
    struct Node_t* right;
    int balance_factor;
    int value;
    Word_ word;
} Node_t;

typedef struct node_stack
{
    Node_t* current;
    struct node_stack* next;
    struct node_stack* prev;
} node_stack;

Node_t *root = NULL;

void print_stack(node_stack* head)
{
    printf("\nstack: ");
    node_stack* temp = head;
    while(temp != NULL)
    { 
        printf("%d ", temp->current->value);
        temp = temp->next;
    }
    printf("\n");

}

int min(int a, int b)
{
    if (a > b)
        return b;
    return a;    
}

int max(int a, int b)
{
    if (a > b)
        return a;
    return b;    
}

node_stack* create_stack_el(Node_t* node)
{
    node_stack* new_stack_el = (node_stack*)malloc(sizeof(node_stack));

    new_stack_el->current = node;
    new_stack_el->next = NULL;
    new_stack_el->prev = NULL;

    return new_stack_el;
}

node_stack* stack_insert(node_stack* head, Node_t* node)
{
    node_stack* new_stack_el = create_stack_el(node);
    node_stack* temp = head;

    while(temp->next != NULL)
    {
        temp = temp->next;
    }

    temp->next = new_stack_el;
    new_stack_el->prev = temp;

    return head;
}

node_stack* get_tail(node_stack* head)
{
    node_stack* temp = head;

    while(temp->next != NULL)
    {
        temp = temp->next;
    }

    return temp;
}

node_stack* get_last_ancestor_stack(node_stack* head)
{
    node_stack* temp = head;

    while(temp->next->next != NULL)
    {
        temp = temp->next;
    }

    return temp;
}


int get_balance_factor(Node_t* node)
{
    if (!node)
        return 0;
    return node->balance_factor;
}

Node_t* create_node(int value, char word[WORD_LEN])
{
    Node_t* new_node = (Node_t*)malloc(sizeof(Node_t));

    new_node->right = NULL;
    new_node->left = NULL;
    new_node->balance_factor = 0;
    new_node->value = value;
    strcpy(new_node->word.word, word);
    new_node->word.chars_count = strlen(word);

    return new_node;
}

/* Prints a tree in preorder traversal.
 * root: root of given tree.
 * Recursive function.
 */
void print_preorder(Node_t *current)
{
    if (current != NULL)
    {
        printf("[%d, %d, %s, %d]", current->value,
                                    current->balance_factor,
                                    current->word.word,
                                    current->word.chars_count);
        print_preorder(current->left);
        print_preorder(current->right);
    }
}

Node_t* rotate_left(Node_t* loc_root, Node_t *rotation_node, Node_t *parent)
{
    Node_t *right_child = rotation_node->right;
    rotation_node->right = right_child->left;
    right_child->left = rotation_node;
    if (parent)
        if (parent->left == rotation_node)
            parent->left = right_child;
        else if (parent->right == rotation_node)
            parent->right = right_child;

    // Fixing balance factors.
    rotation_node->balance_factor += 1 - min(get_balance_factor(right_child), 0);
    right_child->balance_factor += 1 + max(get_balance_factor(rotation_node), 0);

    if (!parent)
        return right_child;
    return loc_root;
}

Node_t* rotate_right(Node_t* loc_root, Node_t *rotation_node, Node_t *parent)
{
    Node_t *left_child = rotation_node->left;
    rotation_node->left = left_child->right;
    left_child->right = rotation_node;

    if (parent)
        if (parent->left == rotation_node)
            parent->left = left_child;
        else if (parent->right == rotation_node)
            parent->right = left_child;

    // Fixing balance factors.
    rotation_node->balance_factor += -1 - max(get_balance_factor(left_child), 0);
    left_child->balance_factor += -1 + min(get_balance_factor(rotation_node), 0);

    if (!parent)
        return left_child;
    return loc_root;
}

Node_t* rebalance(Node_t* loc_root, Node_t* node_to_rebalace, Node_t* parent)
{
    if (node_to_rebalace->balance_factor < 0)
    {
        if (node_to_rebalace->right->balance_factor > 0)
        {
            // Left-right.
            loc_root = rotate_right(loc_root, node_to_rebalace->right, node_to_rebalace);
            return rotate_left(loc_root, node_to_rebalace, parent);
        } else
        {
            // Single left.
            return rotate_left(loc_root, node_to_rebalace, parent);
        }
    } else if (node_to_rebalace->balance_factor > 0)
    {
        if (node_to_rebalace->left->balance_factor < 0)
        {
            // Right-left.
            loc_root = rotate_left(loc_root, node_to_rebalace->left, node_to_rebalace);
            return rotate_right(loc_root, node_to_rebalace, parent);
        } else
        {
            // Single right.
            return rotate_right(loc_root, node_to_rebalace, parent);
        }
    }
    return loc_root;
}

Node_t* update_balance(Node_t* loc_root, node_stack* tail, int type, int side)
{
    node_stack* current_stack_el = tail;
    while (1)
    {
        if (current_stack_el->current->balance_factor > 1 || current_stack_el->current->balance_factor < -1)
        {
            if (current_stack_el->prev)
            {
                loc_root = rebalance(loc_root, current_stack_el->current, current_stack_el->prev->current);
            }
            else
            {
                loc_root = rebalance(loc_root, current_stack_el->current, NULL);
            }
        }
        if (current_stack_el->prev != NULL)
        {
            if (type == DELETION)
            {
                if (current_stack_el->prev->current->left == current_stack_el->current || side == LEFT)
                    current_stack_el->prev->current->balance_factor -= 1;
                else if (current_stack_el->prev->current->right == current_stack_el->current || side == RIGHT)
                    current_stack_el->prev->current->balance_factor += 1;
                side = NO_SIDE;
            } else if (type == INSERTION)
            {
                if (current_stack_el->prev->current->left == current_stack_el->current)
                    current_stack_el->prev->current->balance_factor += 1;
                else if (current_stack_el->prev->current->right == current_stack_el->current)
                    current_stack_el->prev->current->balance_factor -= 1;
            }
            if (current_stack_el->prev->current->balance_factor != 0)
            {
                current_stack_el = current_stack_el->prev;
            } else
            {
                return loc_root;
            }
        } else
        {
            // No nodes to rebalance.
            return loc_root;
        }   
    }
    return loc_root;
}

Node_t* insert_node(Node_t* loc_root, int data, char word[WORD_LEN])
{
    Node_t* node_to_insert = create_node(data, word);
    Node_t* temp_node = loc_root;

    // Stack of nodes.
    node_stack* stack_head = create_stack_el(loc_root);

    // Inserted node is the root.
    if (loc_root == NULL)
        return node_to_insert;

    // Find a place where to insert.
    while (temp_node != NULL)
    {
        if (data > temp_node->value)
        {
            if (temp_node->right == NULL)
            {
                temp_node->right = node_to_insert;
                // Updating balance factors of parents.
                stack_head = stack_insert(stack_head, node_to_insert);
                loc_root = update_balance(loc_root, get_tail(stack_head), INSERTION, NO_SIDE);
                return loc_root;
            }
            temp_node = temp_node->right;
        } else if (data < temp_node->value)
        {
            if (temp_node->left == NULL)
            {
                temp_node->left = node_to_insert;
                // Updating balance factors of parents.
                stack_head = stack_insert(stack_head, node_to_insert);
                loc_root = update_balance(loc_root, get_tail(stack_head), INSERTION, NO_SIDE);
                return loc_root;
            }
            temp_node = temp_node->left;
        }
        else
        {
            // There is already such node with given data.
            return loc_root;
        }
        stack_head = stack_insert(stack_head, temp_node);
    }

    return loc_root;
}

Node_t* find_node(Node_t* loc_root, int data, node_stack* head)
{
    Node_t* temp_node = loc_root;

    while (temp_node != NULL)
    {
        if (data > temp_node->value)
        {
            temp_node = temp_node->right;
        } else if (data < temp_node->value)
        {
            temp_node = temp_node->left;
        }
        else
        {
            // Node is found.
            printf("\nNode with value '%d' is found: '%s', '%d'", data,
                                                                temp_node->word.word,
                                                                temp_node->word.chars_count);
            return temp_node;
        }

        if (head)
            head = stack_insert(head, temp_node);
    }

    // Node is not found.
    printf("\nNode with value '%d' is not found", data);
    return NULL;
}

Node_t* remove_node(Node_t* loc_root, int data)
{
    node_stack* stack_head = create_stack_el(loc_root);
    Node_t* node_to_remove = find_node(loc_root, data, stack_head);
    int side;

    if (node_to_remove == NULL)
        return loc_root;

    Node_t* replace_parent;
    
    if (node_to_remove->right != NULL && node_to_remove->left != NULL)
    {
        // We choose the min of right subtree.
        Node_t* replace_node = node_to_remove->right;
        while (replace_node->left != NULL)
        {
            stack_head = stack_insert(stack_head, replace_node);
            replace_node = replace_node->left;
        }
        stack_head = stack_insert(stack_head, replace_node);
        // Swap values.
        int temp = node_to_remove->value;
        node_to_remove->value = replace_node->value;
        replace_node->value = temp;
        node_to_remove = replace_node;
    }
    replace_parent = get_last_ancestor_stack(stack_head)->current;
    if (node_to_remove->left == NULL && node_to_remove->right == NULL)
    {
        // If node_to_remove have no childs.
        if (replace_parent->left == node_to_remove)
        {
            replace_parent->left = NULL;
            side = LEFT;
        } else if (replace_parent->right == node_to_remove)
        {
            replace_parent->right = NULL;
            side = RIGHT;
        }
        printf("\nNode with data: '%s', '%d' is removed.", node_to_remove->word.word,
                                                            node_to_remove->word.chars_count);
        node_to_remove = NULL;
    } else
    {
        // If node_to_remove have one child.
        Node_t* temp_subtree;
        if (node_to_remove->right != NULL)
        {
            temp_subtree = node_to_remove->right;
        } else if (node_to_remove->left != NULL)
        {
            temp_subtree = node_to_remove->left;
        }
        if (replace_parent->left == node_to_remove)
        {
            replace_parent->left = temp_subtree;
            side = LEFT;
        } else if (replace_parent->right == node_to_remove)
        {
            replace_parent->right = temp_subtree;
            side = RIGHT;
        }
        printf("\nNode with data: '%s', '%d' is removed.", node_to_remove->word.word,
                                                            node_to_remove->word.chars_count);
        node_to_remove = NULL;
    }

    node_stack* tail = get_tail(stack_head);

    loc_root = update_balance(loc_root, tail, DELETION, side);
    return loc_root;
}

int main(void)
{
    // 1 // Big right rotation

    root = insert_node(root, 500, "bus");
    root = insert_node(root, 1000, "car");
    root = insert_node(root, 250, "hello");
    root = insert_node(root, 125, "programming");
    root = insert_node(root, 350, "algorithms");
    root = insert_node(root, 300, "notebook");

    // 2 // Big left rotation

    // root = insert_node(root, 500, "notebook");
    // root = insert_node(root, 250, "notebook");
    // root = insert_node(root, 1000, "programming");
    // root = insert_node(root, 750), "tree";
    // root = insert_node(root, 2000, "AVL");
    // root = insert_node(root, 850, "notebook");

    // 3 //

    // root = insert_node(root, 500, "programming");
    // root = insert_node(root, 250, "data");
    // root = insert_node(root, 1000, "programming");
    // root = insert_node(root, 750, "science");
    // root = insert_node(root, 2000, "windows");
    // root = insert_node(root, 850, "sky");
    // root = insert_node(root, 50, "notes");
    // root = insert_node(root, 3000, "mouse");

    // 4 //

    // root = insert_node(root, 500, "root");
    // root = insert_node(root, 400, "programming");
    // root = insert_node(root, 1000, "node");
    // root = insert_node(root, 750, "value");
    // root = insert_node(root, 700, "key");

    // Removing //

    // 5 //

    // root = insert_node(root, 500, "binary");
    // root = insert_node(root, 400, "number");
    // root = insert_node(root, 1000, "double");
    // root = insert_node(root, 750, "world");
    // root = insert_node(root, 700, "programming");

    // root = remove_node(root, 400);
    // root = remove_node(root, 750);
    // root = remove_node(root, 700);

    // 6 //

    // root = insert_node(root, 500, "name");
    // root = insert_node(root, 250, "surname");
    // root = insert_node(root, 1000, "programming");
    // root = insert_node(root, 750, "phone");
    // root = insert_node(root, 2000, "Horspool");
    // root = insert_node(root, 850, "algorithm");
    // root = insert_node(root, 50, "ceiling");
    // root = insert_node(root, 3000, "programming");

    // root = remove_node(root, 2000);
    // root = remove_node(root, 3000);
    // root = remove_node(root, 1);

    // Search //
    printf("\n---------------\n");

    Node_t* to_find;
    // to_find = find_node(root, 500, NULL);
    // to_find = find_node(root, 1, NULL);
    // to_find = find_node(root, 750, NULL);
    
    printf("\n---------------\n");

    printf("\nEND\n");
    printf("Preorder traversal: ");
    print_preorder(root);
    return 0;
}