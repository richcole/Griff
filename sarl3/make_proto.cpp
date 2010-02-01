#include <stdio.h>
#include <ctype.h>

enum States {
  BOL,
  SKIP_TO_EOL,
  FINISHED,
  ARG_LIST,
  BRACE_STATE,
  COMMENT_STATE,
  ERROR_STATE
};

char *skip_white_space(char *c)
{
  while( *c && isspace(*c) ) ++c;
  return c;
};

int main() 
{
  States state = BOL;
  States prev_state = BOL;

  char buffer[4096 * 12];
  int  pos;
  int  paren_depth = 0;
  int  brace_depth = 0;
  int  c;

  while( state != FINISHED ) {
    switch(state) {
    case SKIP_TO_EOL: // loop until end of line
      switch(c = getchar()) {
      case '\n':
        state = BOL; break; // begining of line
      case EOF:
        state = FINISHED; break; // finished
      }
      break;
    case COMMENT_STATE:
      switch(c = getchar()) {
      case '*':
        switch (c = getchar()) {
        case EOF:
          state = FINISHED; break;
        case '/':
          state = prev_state; break;
        }
        break;
      }
      break;
    case BOL:
      switch(c = getchar()) {
      case '#':
        state = SKIP_TO_EOL; break;
      case ';':
        paren_depth = 0;
        pos = 0; 
        break;
      case '(':
        buffer[pos++] = c;
        paren_depth = 1;
        state = ARG_LIST; 
        break;
      case '{':
        state = BRACE_STATE;
        brace_depth = 1;
        break;
      case EOF:
        state = FINISHED; break; // finished
      case '/':
        switch(c = getchar()) {
        case EOF:
          state = FINISHED; break;
        case '*':
          prev_state = state;
          state = COMMENT_STATE;
        }
        break;
      default:
        buffer[pos++] = c;
      }
      break;
    case BRACE_STATE:
      switch(c = getchar()) {
      case '{':
        brace_depth += 1;
        break;
      case '}':
        brace_depth -= 1;
        if ( brace_depth == 0 ) {
          state = BOL;
        }
        break;
      case '/':
        switch(c = getchar()) {
        case EOF:
          state = FINISHED; break;
        case '*':
          prev_state = state;
          state = COMMENT_STATE;
        }
        break;
      case EOF:
        state = FINISHED; break; // finished
      }
      break;
    case ARG_LIST: 
      switch(c = getchar()) {
      case '(':
        buffer[pos++] = c;
        paren_depth += 1;
        break;
      case ')':
        buffer[pos++] = c;
        paren_depth -= 1;
        if ( paren_depth == 0 ) {
          buffer[pos] = 0;
          printf("%s;\n\n", skip_white_space(buffer));
          state = BOL;
        }
        break;
      case ';':
        state = ERROR_STATE;
        break;
      case '/':
        switch(c = getchar()) {
        case EOF:
          state = FINISHED; break;
        case '*':
          prev_state = state;
          state = COMMENT_STATE;
        }
        break;
      case EOF:
        state = FINISHED; break; // finished
      default:
        buffer[pos++] = c;
      }
      break;
    case ERROR_STATE:
      fprintf(stderr, "Parse Error!\n");
      state = FINISHED;
      break;
    }
  }
}
    
    
