#define _XOPEN_SOURCE 600
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#define __USE_BSD
#include <termios.h>
#include <errno.h>
#include <unistd.h>
#include <assert.h>

/**
 * From code on http://rachid.koucha.free.fr/tech_corner/pty_pdip.html 
 */

int main(){
	int fdm, fds, rc;
	fdm = posix_openpt(O_RDWR);
	assert(!(fdm < 0));
	rc = grantpt(fdm);
	assert(rc == 0);
	rc = unlockpt(fdm);
	assert(rc == 0);
	fds = open(ptsname(fdm), O_RDWR);
	char input[150];
	if(fork()){
		// father process
		close(fds);
		while(1){
			write(1, "Input: ", sizeof("Input: "));
			rc = read(0, input, sizeof(input));
			if(rc > 0){
				write(fdm, input, rc);
				rc = read(fdm, input, sizeof(input)-1);
				if(rc > 0){
					input[rc] = '\0';
					fprintf(stderr, "%s", input);
				}else{
					break;
				}
			}else{
				break;
			}
		}
	}else{
		// child process
		struct termios slave_orig_term_settings;
		struct termios new_term_settings;
		
		close(fdm);
		
		rc = tcgetattr(fds, &slave_orig_term_settings);
		new_term_settings = slave_orig_term_settings;
		cfmakeraw(&new_term_settings);
		tcsetattr(fds, TCSANOW, &new_term_settings);

		close(0); close(1); close(2);
		dup(fds);
		dup(fds);
		dup(fds);
		
		while(1){
			rc = read(fds, input, sizeof(input)-1);
			if(rc > 0){
				input[rc - 1] = '\0';
				printf("Child received: %s\n", input);
			}else{
				break;
			}
		}
	}
	return 0;
}
