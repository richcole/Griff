int main(int argc, char **argv)
{
  if ( argc != 1 ) {
    fprintf(stderr, 
      "Usage: clarify_context <context>\n"
      "  <context> must be a formal context in Burmeister format.\n"
    );
  };
};
