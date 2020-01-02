typedef struct osid_s {
        int iswow64;
        int major;
        int minor;
        int build;
        char hostname[11];//gbk
} osid,*posid;

int parse_request(char *buf,posid poi);
