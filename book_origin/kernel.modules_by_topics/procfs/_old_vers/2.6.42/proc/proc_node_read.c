#include "common.h"

static ssize_t proc_node_read( char *buffer, char **start, off_t off,
                               int count, int *eof, void *data ) {
   static char buf_msg[ LEN_MSG + 1 ] =
          ".........1.........2.........3.........4.........5.........6\n";
   LOG( "read: %d (buffer=%p, off=%ld)", count, buffer, off );
   strcpy( buffer, buf_msg );
   LOG( "return bytes: %d%s", strlen( buf_msg ), *eof != 0 ? " ... EOF" : "" );
   return strlen( buf_msg );
};
