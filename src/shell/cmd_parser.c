#include "cmd_parser.h"
#include "str.h"
#include "vector.h"
#include "util.h"

#include <stdbool.h>

// ======================================================================
// Token
// ======================================================================

typedef enum TokenType {
  // Non terminal tokens.
  TOKEN_LITERAL, // sample
  TOKEN_VARIABLE, // $sample
  TOKEN_ENV_UPDATER, // A=3
  // Terminal tokens.
  TOKEN_SEMI_COLON,
  TOKEN_BAR,
  TOKEN_AND,
  TOKEN_BACKGROUND
} TokenType;

// A token.
// For non-terminal tokens, value is not used.
typedef struct Token {
  TokenType type;
  String *value;
} Token;

Token *token_new(TokenType type, String *value) {
  Token *token = malloc(sizeof(Token));
  token->type = type;
  token->value = value;

  return token;
}

Token *token_from(TokenType type, const char *value) {
  return token_new(type, string_from(value));
}

void token_destroy(void *obj) {
  Token *token = obj;
  if (token->value) {
    free(token->value);
  }
  free(token);
}

const char *token_raw_value(const Token *token) {
  return string_raw(token->value);
}

void token_to_string(const Token *token, String *out) {
  switch (token->type) {
  case TOKEN_LITERAL:
    string_append(out, "Token literal: '");
    string_append(out, token_raw_value(token));
    string_append(out, "'");
    break;
  case TOKEN_VARIABLE:
    string_append(out, "Token variable: '");
    string_append(out, token_raw_value(token));
    string_append(out, "'");
    break;
  case TOKEN_ENV_UPDATER:
    string_append(out, "Token env update: '");
    string_append(out, token_raw_value(token));
    string_append(out, "'");
    break;
  case TOKEN_SEMI_COLON:
    string_append(out, "Token terminal ';'");
    break;
  case TOKEN_AND:
    string_append(out, "Token terminal '&&'");
    break;
  case TOKEN_BAR:
    string_append(out, "Token terminal '|'");
    break;
  case TOKEN_BACKGROUND:
    string_append(out, "Token terminal '&'");
    break;
  }
}


// ======================================================================
// Parser Internal
// ======================================================================

typedef enum LexState {
  QUOTING = 0b1,
  BACKSLASH = 0b10,
  REDIRECT_PENDING = 0b100,
  VARIABLE = 0b1000,
  ENV_UPDATE = 0b10000,
  AND_PENDING = 0b100000
} LexState;

void push_token(Vector *tokens, String *buffer, LexState *state) {
  info("Push token '%s'", string_raw(buffer));
  if (*state & VARIABLE) {
    vector_push(tokens, token_new(TOKEN_VARIABLE, buffer));
    *state &= ~VARIABLE;
  }
  else if (*state & ENV_UPDATE) {
    vector_push(tokens, token_new(TOKEN_ENV_UPDATER, buffer));
    *state &= ~ENV_UPDATE;
  }
  else {
    vector_push(tokens, token_new(TOKEN_LITERAL, buffer));
  }
  buffer = string_new();
}

// Lexical parser.
// 
bool lex_parse(const String *input, Vector *tokens) {

  String *buffer = string_new();
  bool return_code = true;

  LexState state = 0;

  for (size_t i = 0; i < string_length(input); ++i) {
    char ch = string_at(input, i);

    if (state & BACKSLASH) {
      info("Leave BACKSLASH state");
      string_push(buffer, ch);
      state &= ~BACKSLASH;
    }
    else if (ch == '\\') {
      // Enter BACKSLASH state.
      state |= BACKSLASH;
    }
    else if (ch == '"') {
      // Switch QUOTING state.
      if (state & QUOTING) {
        state &= ~QUOTING;
      }
      else {
        state |= QUOTING;
      }
    }
    else if (ch == '$' && !(state & VARIABLE)) {
      if (!string_empty(buffer)) {
        push_token(tokens, buffer, &state);
        buffer = string_new();
      }
      string_push(buffer, ch);
      state |= VARIABLE;
    }
    else if (state & QUOTING) {
      // In quoting state, just push current char to buffer.
      string_push(buffer, ch);
    }
    else if (ch == '=') {
      state |= ENV_UPDATE;
    }
    else if (ch == '&' && (state & AND_PENDING)) {
      vector_push(tokens, token_new(TOKEN_AND, string_from("&&")));
      buffer = string_new();
    }
    else if (ch != '&' && (state & AND_PENDING)) {
      vector_push(tokens, token_new(TOKEN_BACKGROUND, string_from("&")));
      buffer = string_new();
    }
    // TODO add redirect support
    else if (ch == ';' || ch == ' ' || ch == '|' || ch == '&'
             || ch == '>' || ch == '<') {
      // Terminal tokens.
      if (!string_empty(buffer)) {
        push_token(tokens, buffer, &state);
        buffer = string_new();
      }
    }
    else {
      string_push(buffer, ch);
    }
  }

  if (!string_empty(buffer)) {
    // TODO check terminal state
    push_token(tokens, buffer, &state);
    buffer = string_new();
  }

  return return_code;
}

// ======================================================================
// Syntax Parser
// ======================================================================

void syntax_parse(Vector *tokens, Vector *commands) {
  
}

// ======================================================================
// API
// ======================================================================

bool parse(const String *input) {
  Vector *tokens = vector_new(token_destroy);

  if (!lex_parse(input, tokens)) {
    return false;
  }

  info("Tokens:");
  String *buffer = string_new();
  for (int i = 0; i < vector_size(tokens); i++) {
    token_to_string(vector_at(tokens, i), buffer);
    info(" | %s", string_raw(buffer));
    string_clear(buffer);
  }
  string_destroy(buffer);

  // syntax_parse(tokens, )

  vector_destroy(tokens);

  return true;
}
