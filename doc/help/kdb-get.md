kdb-get(1) -- Get the value of a key stored in the key database
================================================================

## SYNOPSIS

`kdb get <path>`  

Where `path` is the full path to the key.  

## DESCRIPTION

This command is used to retrieve the value of a key.  

If you enter a `path` starting with a leading `/`, then a cascading lookup will be performed in order to attempt to locate the key.  
In this case, using the `-v` option allows the user to see the full `path` of the key if it is found.  

Note: There is a current limitation where only keys that are mounted will be considered during a cascading lookup.  
A workaround that will lookup all keys is to pass the `-a` option.  
Additionally, a user can use the command `kdb ls <same key>` to see if an override or a fallback will be considered by the lookup.  

## OPTIONS

- `-H`, `--help`:
  Show the man page.
- `-V`, `--version`:
  Print version info.
- `-a`, `--all`:
  Consider all of the keys.
- `-n`, `--no-newline`:
  Suppress the newline at the end of the output.
- `-v`, `--verbose`:
  Explain what is happening.
  Gives a complete trace of all tried keys.

## EXAMPLES

To get the value of a key:  
	`kdb get user/example/key`  

To get the value of a key using a cascading lookup:  
	`kdb get /example/key`  

To get the value of a key without adding a newline to the end of it:
	`kdb get -n /example/key`

To explain why a specific key was used (for cascading keys):
	`kdb get -v /example/key`

## SEE ALSO

- For more about cascading keys see [elektra-cascading(7)](elektra-cascading.md)
- TO get keys in shell scripts, you also can use [kdb-sget(1)](kdb-sget.md)