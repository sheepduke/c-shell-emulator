#include "cmd_parser.h"
#include "command.h"
#include "str.h"
#include "vector.h"
#include "util.h"

#include <assert.h>
#include <stdbool.h>

// ======================================================================
// Token
// ======================================================================

typedef enum TokenType {
  // Non terminal tokens.
  TOKEN_LITERAL, // sample
  TOKEN_VARIABLE, // $sample
  TOKEN_ENV_UPDATER_NAME, // "A" inside "A=3"
  TOKEN_ENV_UPDATER_VALUE, // "3" inside "A=3"
  // Terminal tokens.
  TOKEN_SEMI_COLON,
  TOKEN_BAR,
  TOKEN_AND,
  TOKEN_BACKGROUND,
  TOKEN_REDIRECT_SOURCE,
  TOKEN_INPUT_REDIRECT_FILE,
  TOKEN_INPUT_REDIRECT_STREAM,
  TOKEN_OUTPUT_REDIRECT_FILE,
  TOKEN_OUTPUT_REDIRECT_APPEND,
  TOKEN_OUTPUT_REDIRECT_STREAM
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

TokenType token_type(const Token *token) {
  return token->type;
}

const String *token_value(const Token *token) {
  return token->value;
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
  case TOKEN_ENV_UPDATER_NAME:
    string_append(out, "Token env update name: '");
    string_append(out, token_raw_value(token));
    string_append(out, "'");
    break;
  case TOKEN_ENV_UPDATER_VALUE:
    string_append(out, "Token env update value: '");
    string_append(out, token_raw_value(token));
    string_append(out, "'");
    break;
  case TOKEN_SEMI_COLON:
    string_append(out, "Token terminal semi_comma ';'");
    break;
  case TOKEN_AND:
    string_append(out, "Token terminal and '&&'");
    break;
  case TOKEN_BAR:
    string_append(out, "Token terminal bar '|'");
    break;
  case TOKEN_BACKGROUND:
    string_append(out, "Token terminal background '&'");
    break;
  case TOKEN_REDIRECT_SOURCE:
    string_append(out, "Token input redirect stream source: '");
    string_append(out, token_raw_value(token));
    string_append(out, "'");
    break;
  case TOKEN_INPUT_REDIRECT_FILE:
    string_append(out, "Token input redirect");
    break;
  case TOKEN_INPUT_REDIRECT_STREAM:
    string_append(out, "Token input redirect");
    break;
  case TOKEN_OUTPUT_REDIRECT_FILE:
    string_append(out, "Token output redirect to file");
    break;
  case TOKEN_OUTPUT_REDIRECT_STREAM:
    string_append(out, "Token output redirect to stream");
    break;
  case TOKEN_OUTPUT_REDIRECT_APPEND:
    string_append(out, "Token output redirect to file append");
    break;
  }
}


// ======================================================================
// Parser Internal
// ======================================================================

typedef enum LexState {
  SINGLE_QUOTING = 0b1,
  DOUBLE_QUOTING = 0b10,
  BACKSLASH = 0b100,
  REDIRECT_PENDING = 0b1000,
  REDIRECT_STREAM = 0b10000,
  VARIABLE = 0b100000,
  ENV_UPDATE_NAME = 0b1000000,
  ENV_UPDATE_VALUE = 0b10000000,
  AND_PENDING = 0b100000000
} LexState;

void push_token(Vector *tokens, String *buffer, LexState *state) {
  if (string_empty(buffer)) {
    return ;
  }
  if (*state & VARIABLE) {
    vector_push(tokens, token_new(TOKEN_VARIABLE, buffer));
    *state &= ~VARIABLE;
  }
  else if (*state & ENV_UPDATE_NAME) {
    vector_push(tokens, token_new(TOKEN_ENV_UPDATER_NAME, buffer));
    *state &= ~ENV_UPDATE_NAME;
  }
  else if (*state & ENV_UPDATE_VALUE) {
    vector_push(tokens, token_new(TOKEN_ENV_UPDATER_VALUE, buffer));
    *state &= ~ENV_UPDATE_VALUE;
  }
  else {
    vector_push(tokens, token_new(TOKEN_LITERAL, buffer));
  }
}

// Check terminal state of state machine.
// 
bool check_terminal_state(LexState state) {
  if (state & BACKSLASH) {
    error("No char after backslash '\\'");
    return false;
  }
  else if (state & SINGLE_QUOTING) {
    error("No matching single quote '''");
    return false;
  }
  else if (state & DOUBLE_QUOTING) {
    error("No matching double quote '\"'");
    return false;
  }
  else if (state & REDIRECT_PENDING) {
    error("I need a file name after token '>' or '<'.");
    return false;
  }
  return true;
}

// Lexical parser.
// 
bool lex_parse(const String *input, Vector *tokens) {

  String *buffer = string_new();
  bool return_code = true;

  LexState state = 0;

  for (size_t i = 0; i < string_length(input); ++i) {
    char ch = string_at(input, i);

    // Just push char to buffer when in state BACKSLASH.
    // 
    if (state & BACKSLASH) {
      info("Leave BACKSLASH state");
      string_push(buffer, ch);
      state &= ~BACKSLASH;
    }
    // Enter BACKSLASH state.
    // 
    else if (ch == '\\') {
      state |= BACKSLASH;
    }
    // Switch DOUBLE_QUOTING state.
    // 
    else if (ch == '"' && !(state & SINGLE_QUOTING)) {
      if (state & DOUBLE_QUOTING) {
        state &= ~DOUBLE_QUOTING;
      }
      else {
        state |= DOUBLE_QUOTING;
      }
    }
    // Switch SINGLE_QUOTING state.
    // 
    else if (ch == '\'' && !(state & DOUBLE_QUOTING)) {
      if (state & SINGLE_QUOTING) {
        state &= ~SINGLE_QUOTING;
      }
      else {
        state |= SINGLE_QUOTING;
      }
    }
    // Match variable.
    // 
    else if (ch == '$' && !(state & SINGLE_QUOTING)) {
      if (!string_empty(buffer)) {
        push_token(tokens, buffer, &state);
        buffer = string_new();
      }
      state |= VARIABLE;
    }
    // Match quoted string.
    // 
    else if (state & DOUBLE_QUOTING || state & SINGLE_QUOTING) {
      // In quoting state, just push current char to buffer.
      string_push(buffer, ch);
    }
    // Match variable definition.
    // 
    else if (ch == '=' && !string_empty(buffer)
             && !(state & ENV_UPDATE_VALUE)) {
        state |= ENV_UPDATE_NAME;
        push_token(tokens, buffer, &state);
        buffer = string_new();
        state |= ENV_UPDATE_VALUE;
    }
    // Match input/output redirect.
    // 
    else if (state & REDIRECT_PENDING) {
      TokenType token_type = TOKEN_OUTPUT_REDIRECT_FILE;
      char last_char = string_last(buffer);
      string_sub(buffer, 0, string_length(buffer)-1);

      assert(last_char == '<' || last_char == '>');

      state &= ~REDIRECT_PENDING;
      if (ch == '&') {
        // Check if existing char in buffer is digit.
        for (int i = 0; i < string_length(buffer); i++) {
          char c = string_at(buffer, i);
          if (c < '0' || c > '9') {
            error("Token before '>&' should be a valid file descriptor.\n"
                    "But '%s' is not.",
                    string_raw(buffer));
            string_destroy(buffer);
            return false;
          }
        }
        vector_push(tokens, token_new(TOKEN_REDIRECT_SOURCE, buffer));
        buffer = string_new();

        token_type = (last_char == '<')
          ? TOKEN_INPUT_REDIRECT_STREAM
          : TOKEN_OUTPUT_REDIRECT_STREAM;
        state |= REDIRECT_STREAM;
      }
      else {
        push_token(tokens, buffer, &state);
        buffer = string_new();
        if (last_char == '<' && ch == '&') {
          token_type = TOKEN_INPUT_REDIRECT_STREAM;
          state |= REDIRECT_STREAM;
        }
        else if (last_char == '>' && ch == '&') {
          token_type = TOKEN_OUTPUT_REDIRECT_STREAM;
          state |= REDIRECT_STREAM;
        }
        else if (last_char == '>' && ch == '>') {
          token_type = TOKEN_OUTPUT_REDIRECT_APPEND;
        }
        else if (ch == '<' || (last_char == '<' && ch == '>')) {
          error("Direction of redirect is really in chaos around '%c'.", ch);
          return_code = false;
          break;
        }
        else {
          --i;
          if (last_char == '<') {
            token_type = TOKEN_INPUT_REDIRECT_FILE;
          }
          else if (last_char == '>') {
            token_type = TOKEN_OUTPUT_REDIRECT_FILE;
          }
          else {
            assert("Wrong state transition: redirection.");
          }
        }
      }
      vector_push(tokens, token_new(token_type, string_new()));
    }
    else if (state & REDIRECT_STREAM) {
      if (ch < '0' || ch > '9') {
        error("I expect a file descriptor after '>&'.\n"
              "Character '%c' should not appear in file descriptor.",
              ch);
        return_code = false;
        break;
      }
      else {
        string_push(buffer, ch);
      }
    }
    // Match '&' or '&&'.
    // 
    else if (ch == '&' && (state & AND_PENDING)) {
      string_push(buffer, '&');
      vector_push(tokens, token_new(TOKEN_AND, buffer));
      buffer = string_new();
    }
    else if (ch != '&' && (state & AND_PENDING)) {
      vector_push(tokens, token_new(TOKEN_BACKGROUND, buffer));
      buffer = string_new();
    }
    // Match the start of input/output redirect.
    // 
    else if (ch == '>' || ch == '<') {
      state |= REDIRECT_PENDING;
      string_push(buffer, ch);
    }
    // Match terminal tokens.
    // 
    else if (ch == ';' || ch == ' ' || ch == '|' || ch == '&') {
      if (!string_empty(buffer)) {
        push_token(tokens, buffer, &state);
        buffer = string_new();
      }
      if (ch == ';') {
        vector_push(tokens, token_new(TOKEN_SEMI_COLON, string_from(";")));
      }
      else if (ch == '|') {
        vector_push(tokens, token_new(TOKEN_BAR, string_from("|")));
      }
      else if (ch == '&') {
        state |= AND_PENDING;
        string_push(buffer, ch);
      }
    }
    else {
      string_push(buffer, ch);
    }
  }

  // check terminal state
  if (return_code) {
    return_code = check_terminal_state(state);
  }
  if (return_code && !string_empty(buffer)) {
    push_token(tokens, buffer, &state);
  }
  else {
    string_destroy(buffer);
  }
  
  return return_code;
}


// ======================================================================
// Syntax Parser
// ======================================================================

typedef enum SyntaxState {
  SYNTAX_NONE,
  SYNTAX_ENV,
  SYNTAX_COMMAND,
  SYNTAX_REDIRECTION
} SyntaxState;

bool syntax_parse(Vector *tokens, Vector *commands) {
  SyntaxState state = SYNTAX_NONE;

  // Vector *or_commands = vector_new(command_destroy);
  // Vector *and_commands = vector_new(vector_destroy);
  // Vector *piped_commands = vector_new(vector_destroy);
  Command *command = NULL;

  bool return_code = true;

  String *EXPORT = string_from("export");
  bool export_flag = false;

  for (int i = 0; return_code && i < vector_size(tokens); i++) {
    const Token *token = vector_at(tokens, i);
    TokenType type = token_type(token);
    const String *value = token_value(token);

    if (state == SYNTAX_NONE && type == TOKEN_LITERAL) {
      if (string_equal(value, EXPORT)) {
        command = command_new(string_clone(value));
        export_flag = true;
      }
      else {
        state = SYNTAX_COMMAND;
        command = command_new(string_clone(value));
      }
    }
    else if (export_flag && type == TOKEN_ENV_UPDATER_NAME) {
      command_push_arg(command, string_clone(value));
    }
    else if (export_flag && type == TOKEN_ENV_UPDATER_VALUE) {
    }
    else if (state == SYNTAX_NONE && type == TOKEN_ENV_UPDATER_NAME) {
      state = SYNTAX_ENV;
    }
    else if (state == SYNTAX_COMMAND && type == TOKEN_LITERAL) {
       command_push_arg(command, string_clone(value));
    }
    else if (state == SYNTAX_COMMAND && type == TOKEN_VARIABLE) {
      
    }
  }

  string_destroy(EXPORT);
  return return_code;
}

// ======================================================================
// API
// ======================================================================

bool parse(const String *input, Vector *commands) {
  Vector *tokens = vector_new(token_destroy);

  if (!lex_parse(input, tokens)) {
    error("Lexical parser returned false.");
    vector_destroy(tokens);
    return false;
  }
  
  debug("Tokens:");
  String *buffer = string_new();
  for (int i = 0; i < vector_size(tokens); i++) {
    token_to_string(vector_at(tokens, i), buffer);
    info(" | %s", string_raw(buffer));
    string_clear(buffer);
  }
  string_destroy(buffer);

  if (!syntax_parse(tokens, commands)) {
    error("Syntax parse returned false.");
    vector_destroy(tokens);
    return false;
  }
  debug("Commands:");

  vector_destroy(tokens);

  return true;
}
