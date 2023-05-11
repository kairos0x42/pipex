#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>


/*
s :: The string to be split
c :: The delimiter character
char **split_42(const char *s, char c);

return value :: array of new strings resulting from the split

:: Allocates with malloc() and returns an array of strings obtained by splitting
's' using the character 'c' as a delimiter. The array must be ended by a NULL
pointer.
*/

size_t strlen_99(const char *str) {
    size_t len = 0;
    while (str[len] != '\0') {
        len++;
    }

    return len;
}


char *strncpy_99(char *dst, const char *src, size_t n) {
    size_t i = 0;
    for (; i < n && src[i] != '\0'; i++) {
        dst[i] = src[i];
    }
    dst[i] = '\0';


    return dst;

}



char **split_42(const char *s, char c) {
    size_t s_len = strlen_99(s);

    // Count number of tokens in string
    size_t token_count = 1;
    for (size_t i = 0; i < s_len; i++) {
        if (s[i] == c)
            token_count++;
    }

    /*
    :: Allocate memory for new_strings
    :: +1 for a sentinel value, ie the last token is going to be a null pointer
    */
    char **new_strings = malloc((token_count +1) * sizeof(char *));
    if (!new_strings) {
        return NULL;
    }

    // NOTE:: the token is the substring in the string

    // Track starting index for token
    // Track the first index for the token
    size_t first_i = 0;

    // Track current token index, this is the index value for the new_strings char **arr
    size_t token_idx = 0;

    /*
    Example:: 
    :: Delimeter is ':'
    tree:table:the

    :: ':' is at index 4 and 10
    */

    for (size_t i = 0; i < s_len; i++) {
        if (s[i] == c) {
            // length of token
            /* i = 4 on the first hit */
            size_t token_len = i - first_i;
            
            /*
            :: Second allocation
            :: Allocate memory for the current token, including space for the null-terminator
            */
            /* token_idx tracks row number in matrix */
            new_strings[token_idx] = malloc(token_len +1);
            if (new_strings[token_idx] == NULL) {
                return NULL;
            }

            // Copy token into new_strings[token_idx]
            strncpy_99(new_strings[token_idx], &s[first_i], token_len);

            // null terminate token
            new_strings[token_idx][token_len] = '\0';

            // Set first_i to be the character after the character found
            // Remember that this is the 'index' for the first character of the token
            first_i = i + 1;

            // Increment the token index
            token_idx++;
        }
    }
    // Handle last token
    // subtract the total length of the string from the index value of the first character of the token string
    size_t token_len = s_len - first_i;

    // Allocate memory for the new token
    new_strings[token_idx] = malloc((token_len +1) * sizeof(char));
    if (new_strings[token_idx] == NULL) {
        return NULL;
    }
    strncpy_99(new_strings[token_idx], &s[first_i], token_len);

    // null terminate last token
    new_strings[token_idx][token_len] = '\0';

    /*
    :: the last pointer of **new_strings is a sentinel value, marks the end of
    the end of the array
    */
    new_strings[token_count] = NULL;


    return new_strings;
}


int main(int argc, char *argv[]) {

    int pipefd[2];
    char buf[1000];

    // Create pipe
    if (pipe(pipefd) == -1) {
        return 1;
    }
    // pipefd[0] == Read end of pipe
    // pipefd[1] == Write end of pipe

    // Open up the infile for reading
    int infilefd = open(argv[1], O_RDONLY);

    // Open up the outfile for writing
    int outfilefd = open(argv[4], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    // fd = open("example.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);


    pid_t pid = fork();

    // Parent
    if (pid > 0) {
        close(pipefd[0]);
        // const char *msg = "this is a message for the child process";
        // write(pipefd,msg,strlen(msg));
        // Change stdout to be the write end of the pipe
        dup2(pipefd[1],1);

        // Set stdin to be infile
        dup2(infilefd, 0);

        // The third element is going to be the first command to execute
        char **args = split_42(argv[2], ' ');
        execvp(args[0],args);
        //wait(NULL);

    // Child
    } else {
        close(pipefd[1]);

        // Set stdin to have the same file descriptor 
        dup2(pipefd[0],0);

        // set stdout to be the file descriptor of the outfile
        dup2(outfilefd,1);

        // The command to be run will be the fourth argument passed to the command
        char **args = split_42(argv[3], ' ');
        execvp(args[0],args);


    }

    return 0;
}

