# ctfdiff
CTF diff of two files.

## Build
```
$ ninja
```
### Dependencies
 * libbsdctf

## Usage
```
ctfdiff [-afFhilLoOqtT] file1 file2	
```

### General options
 * `-h` help message
 * `-q` quiet mode 
 * `-i` ignore integral types (allows for equality between 32bit and 64bit
        memory models)
 * `-a` compare all sections

### Label options
 * `-l` compare all labels

### Object options
 * `-o` compare all objects
 * `-O name` compare object with the specified name (may repeat)

### Function options
 * `-f` compare all functions
 * `-F name` compare function with the specified name (may repeat)

### Type options
 * `-t` compare all types
 * `-T name` compare type with the specified name (may repeat)

## Comparison algorithm
### Label
Two labels are equal, if they have the same name.

### Object
Two objects are equal, if they have the same name and type.

### Function
Two functions are equal, if they have the same name, return type and arguments
(both count- and type-wise)

### Type
Two types are equal, if they have the same kind and the variable data
(properties and lists) stored with the type is the same (e.g. member offsets, enum entry values, integer content, float encoding, ...).

## Exit status 
The result of the diff can be determined also by the return code/exit status of the
program.
### 0
The requested diff query contains no differences.
### 1
The requested diff query contains differences.
### 2
Wrong arguments.
### 3
An error occurred during the execution.

## Run
```
$ ctfdiff -q -T 'struct proc' /boot/kernel.old/kernel /boot/kernel/kernel
$ echo $?
0
```

## Authors
 * Daniel Lovasko lovasko@freebsd.org

