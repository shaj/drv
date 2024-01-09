#define LEN_MSG 160
static char buf_msg[LEN_MSG + 1];

static ssize_t node_read(struct file *file, char *buf,
                         size_t count, loff_t *ppos) {
   static bool first = false;
   int res;
   if ((first = !first)) {
      sprintf(buf_msg, "%u", period);
      printk("! return %lu bytes: <%s>\n", strlen(buf_msg), buf_msg);
      strcat(buf_msg, "\n");
      res = copy_to_user((void*)buf, buf_msg, strlen(buf_msg));
      return strlen(buf_msg);
   } else {
      *ppos = 0;
      printk("! return EOF\n");
      return 0;
   }
}

static ssize_t node_write(struct file *file, const char *buf,
                          size_t count, loff_t *ppos) {
   int res;
   char *eptr;
   uint len = count < LEN_MSG ? count : LEN_MSG, input;
   res = copy_from_user(buf_msg, (void*)buf, len);
   buf_msg[len] = '\0';
   input = simple_strtoul(buf_msg, &eptr, 0);
   if (0 == input) return -EINVAL;
   period = input;
   printk("! write: %lu bytes - %u\n", (uintptr_t)(eptr - buf_msg), period);
   return len;
}
