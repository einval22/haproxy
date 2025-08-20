package main

import (
	"bufio"
	"fmt"
	"net"
	"os"
	"time"
)

func main() {
	fmt.Println("Starting observer...")
	for {
		fmt.Println("Start...")
		conn, err := net.Dial("unix", "/home/vk/projects/haproxy/master.sock")
		if err != nil {
			fmt.Fprintf(os.Stdout, "Failed to connect to socket: %v\n", err)
			time.Sleep(1 * time.Second)
			continue
		}

		loop:
		for i := 1; i <= 3; i++ {
			_, err = conn.Write([]byte("mcli-debug-mode on;trace pt sink stderr; trace pt level developer; trace pt verbosity complete; trace pt start now; @1 show stat\n"))
			if err != nil {
				fmt.Fprintf(os.Stdout, "%d: Failed to write to socket: %v\n", i, err)
				break
			}

			fmt.Fprintf(os.Stdout, "%v >>>> Has written req to socket\n", time.Now())

			time.Sleep(7 * time.Second)

			// deadline for reading
			//conn.SetDeadline(time.Now().Add(5 * time.Second))
			r := bufio.NewReader(conn)
			for {
				line, err := r.ReadSlice('\n')
				if err != nil {
					fmt.Fprintf(os.Stdout, "%d: Failed to read from socket: %v\n", i, err)
					break loop
				}
				fmt.Fprintf(os.Stdout, "===== 1. >%s<", line)
				if line[0] == '\n' {
					fmt.Fprintf(os.Stdout, ">>> Done: read all output\n")
					break
				}
			}
		}

		if err := conn.Close(); err != nil {
			fmt.Fprintf(os.Stdout, "Failed to close connection: %v\n", err)
		}
		fmt.Println("++++++++++++++++++++++++...End: send 3 reqs+++++++++++++++++++")
		time.Sleep(2 * time.Second)
	}
}
