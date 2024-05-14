Assingment 2 Bare Metal OS

student number: s3740878
student name: Hur Hyeon Bin

File usages:

kernel.c : includes main()
q1.c: includes functions for task 1 and task 2
uart0.c: includes uart configurations function

Commands:

"help"
"help <command>"
"setcolor -t <color>"
"setcolor -b <color>"
"setcolor -t <color> -b <color>"
"setcolor -b <color> -t<color>"
"showinfo"
"clear"
backspace to delete charicter
tab to autocomplete the command
_ button to track the history working as UP arrow
+ button to track the history working as DOWN arrow
"change baudrates <number>" change buad rate
<number>: 9600, 19200, 38400, 57600, 115200
"change databits <number>" change data bit
<number>: 1, 2
"change stopbits <number>" change stopbit
<number>: 5,6,7,8
"change paritybits <string>" change paritybit
<string>: "none", "odd", "even"
"change handshaking <string>" change handshaking
<string>: "on", "off"

Default UART configuration:

baudrate : 115200
data bit: 7
stop bit: 1
paritybit: none
handshaking: off

Issue:

after changing baudrate to 9600, 19200 or 38400, the further configuration modification may not working.
