#include "../headers/eval.h"
#include "../headers/context.h"
#include "../headers/builtins.h"

#include <stdio.h>


Context eval_context_new() {
    Context ctx = context_new(NULL);

#define CTX_DEF(ctx, name, node)\
    context_define((ctx), string_intern(name), node)

#define CTX_DEF_FUN(ctx, name, func)\
    context_define((ctx), string_intern(name), node_new_nfunc(func))

    CTX_DEF(&ctx, "nil", node_new_nil());

    CTX_DEF_FUN(&ctx, "print", &builtin_print);
    CTX_DEF_FUN(&ctx, "println", &builtin_println);
    CTX_DEF_FUN(&ctx, "def", &builtin_def);
    CTX_DEF_FUN(&ctx, "eval", &builtin_eval);
    CTX_DEF_FUN(&ctx, "lambda", &builtin_lambda);
    CTX_DEF_FUN(&ctx, "car", &builtin_car);
    CTX_DEF_FUN(&ctx, "cdr", &builtin_cdr);
    CTX_DEF_FUN(&ctx, "cons", &builtin_cons);
    CTX_DEF_FUN(&ctx, "+", &builtin_plus);
    CTX_DEF_FUN(&ctx, "-", &builtin_minus);
    CTX_DEF_FUN(&ctx, "*", &builtin_times);
    CTX_DEF_FUN(&ctx, "/", &builtin_div);

#undef CTX_DEF_FUN

    return ctx;
}

void set_ctx_args(Context* ctx, Node* funcargs, Node* args) {
    Node* arg = funcargs;
    Node* value = args;
    while (node_car(arg)->type != NODE_NIL) {
        context_define(ctx, node_car(arg)->symbol, eval(ctx, node_car(value)));
        arg = node_cdr(arg);
        value = node_cdr(value);
    }
}

Node* eval_funcall(Context* ctx, Node* func, Node* args) {
    Context context = context_new(ctx);
    set_ctx_args(&context, func->left, args);
    Node* ast = func->right;
    Node* result = node_new_nil();
    while (ast->type != NODE_NIL) {
        result = eval(&context, ast->left);
        ast = node_cdr(ast);
    }
    return result;
}

Node* eval_list(Context* ctx, Node* node) {
    Node* name = node->left;
    Node* args = node->right;
    if (name->type == NODE_SYMBOL) {
        Node* func = context_get(ctx, name->symbol);
        if (func == NULL) {
            printf("Unable to resolve symbol: %s\n", name->symbol);
            return node_new_nil();
        } else {
            if (func->type == NODE_NATIVE_FUNC) {
                return func->func(ctx, args);
            } else if (func->type == NODE_FUNC) {
                return eval_funcall(ctx, func, args);
            } else {
                printf("%s is not a function.\n", name->symbol);
                return node_new_nil();
            }
        }
    } else {
        printf("Syntax error in funcall. (%d)\n", name->type);
        return node_new_nil();
    }
}

Node* eval_force(Context* ctx, Node* node) {
    switch (node->type) {
        case NODE_NIL:
        case NODE_INTEGER:
        case NODE_NATIVE_FUNC:
        case NODE_FUNC:
            return node;
        case NODE_SYMBOL:
            return context_get(ctx, node->symbol);
        case NODE_PAIR:
            return eval_list(ctx, node);
    }
}

Node* eval(Context* ctx, Node* node) {
    if (node->props & NP_LITERAL) {
        return node;
    } else {
        return eval_force(ctx, node);
    }
}