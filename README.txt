SYSC4001: Assignment 3
Philip Baird and Dimitry Koutchine

NOTE:
start dbserver before atm or dbeditor

COMMENT:
dbserver works by opening a message stream and starting threads for each input message type.
when a message is received on a thread the semaphore is acquired and the required method is called (ie withdraw).
the semaphore is then released
this is then returned to the message stream with the respective return type and returned value from the method.

atm and dbeditor open the same message stream as dbserver and adds messages with types for the required methods.
they then wait on the reply from dbserver (this is built into the msgrcv call with type 0) printing the return value.

