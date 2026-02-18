#!/bin/bash

# *-------------------- Exit Codes --------------------* #
#  0 : Success
#  1 : Runtime/Logic error
#  2 : Configuration error

# Files
makefile=Makefile
required_source_files=("assg.c" "spheres.c" "vector.c" "color.c")
executables=("test_vectors") # Renamed from MS1_assg
program_name="raytracer"     # Renamed from FS_assg

# Compilation and linking flags and time limit
compilation_flags="-Wall -Werror -std=c99 -DFS"
linking_flags="-lm"
time_limit=2

# Required directories
source_dir="./src"
bin_dir="./bin"
testcases_dir="./tests/scenes"     # Renamed from FS_Testcases

# Postfixes for files found or generated in the testcases directory
input_postfix="_input.txt"
output_postfix="_output.ppm"
generated_postfix="_generated.ppm"

# Logging functions and starting message
width=$(tput cols);
log()     { printf "\033[00m[TEST] %s\033[0m\n" "$1"; }
info()    { printf "\033[33m[TEST] %s\033[0m\n" "$1"; }
success() { printf "\033[32m[TEST] %s\033[0m\n" "$1"; }
warning() { printf "\033[35m[TEST] Warning: %s\033[0m\n" "$1"; }
br()      { break=$(printf "%*s" "$((width-8))" "" | tr " " "-"); log "$break"; }
error() {
    printf "\033[31m[TEST] Error: %s\033[0m\n" "$1";
    log ""
    br; info "END: Suite failed."; br; echo;
    exit "$2";
}

doesPass=1

errorCont() {
    doesPass=0
    printf "\033[31m[TEST] Fail: %s\033[0m\n" "$1";
    info "Marking test as failed and continuing..."
}

echo; br; success "STARTING INTEGRATION SUITE"; br; log ""

# Check if required directories exists
if [ ! -d "$testcases_dir" ]; then
    error "Expected test cases directory '$testcases_dir' does not exist." 2
fi
if [ ! -d "$source_dir" ]; then
    error "Expected source directory '$source_dir' does not exist." 1
fi

# Check if ppmcmp.py exists
if [ ! -f "ppmcmp.py" ]; then
    error "Comparison script 'ppmcmp.py' not found in current directory." 1
fi

# Check that all required files exist
for file in "${required_source_files[@]}"; do
    if [ ! -e "$source_dir/$file" ]; then
        error "Required file '$source_dir/$file' not found." 1
    fi
done

if [ ! -e "$makefile" ]; then
    error "$makefile does not exist." 1
fi

# Remove executables if they exist
rm -f "$program_name"
for exe in "${executables[@]}"; do
    rm -f "$exe"
done

# Compile and link using Makefile
log "Building project..."
if ! make -f "$makefile" > /dev/null; then
    error "$makefile failed to run." 1
fi
exe_path="./bin/$program_name"

# Check if the makefile created the executables
if [ ! -e "bin/$program_name" ]; then
    error "$makefile failed to create $program_name." 1
fi
success "Build successful: $program_name created."

log "Running integration tests on '$program_name'..."

# Iterate over every input file corresponding to the exercise number
# We use nullglob in case no files match to avoid literal string iteration
shopt -s nullglob
for input_path in "$testcases_dir"/*"$input_postfix"; do

    log ""

    # Check if the corresponding output file exists. If it doesn't then skip this test case
    output_path=${input_path/"$input_postfix"/"$output_postfix"}
    if [[ ! -e $output_path ]]; then
        info "Missing reference output: '$output_path'. Skipping."
        continue
    fi

    # Set timeout and generate student output file
    generated_output_path=${input_path/"$input_postfix"/"$generated_postfix"}
    log "Processing '$input_path'..."
    
    # Run the program (Removed timeout command to fix Mac issues)
    # If you install 'coreutils', you can change this back to: gtimeout "$time_limit" ...
    "$exe_path" "$input_path" "$generated_output_path"
    
    exit_code=$?
    if [ $exit_code -ne 0 ]; then
        errorCont "Runtime error (Exit Code: $exit_code)." 1
        continue
    else
        success "Execution successful."        
    fi

    # Check for memory leaks (Only if Valgrind is installed)
    if command -v valgrind &> /dev/null; then
        log "Checking memory integrity (Valgrind)..."
        valgrind_output=$(valgrind "$exe_path" "$input_path" "$generated_output_path" 2>&1)
        
        # Parse output with safety defaults
        bytes=$(echo "$valgrind_output" | grep "in use at exit" | awk '{print $6}')
        blocks=$(echo "$valgrind_output" | grep "in use at exit" | awk '{print $9}')
        
        # Default to 0 if empty string
        bytes=${bytes:-0}
        blocks=${blocks:-0}
        # Remove commas (e.g. 1,024 -> 1024)
        bytes=${bytes//,/}
        blocks=${blocks//,/}

        if [ "$bytes" -ne 0 ] || [ "$blocks" -ne 0 ]; then
            errorCont "Memory leak detected: $bytes bytes in $blocks blocks." 1
        else
            success "No memory leaks detected."        
        fi
    else
        log "Skipping Valgrind (not installed)."
    fi

    # Run `ppmcmp` comparison
    # We capture output to a variable but DO NOT suppress stderr so we can see Python errors
    ppmcmp_out=$(python3 ppmcmp.py "$output_path" "$generated_output_path" 2>&1)
    exit_code=$?

    if [ $exit_code -eq 2 ]; then
        errorCont "Invalid PPM format." 1
    elif [ $exit_code -eq 3 ]; then
        errorCont "Max color value should be 255." 1
    elif [ $exit_code -eq 4 ]; then
        errorCont "Incorrect image dimensions." 1
    elif [ $exit_code -eq 5 ]; then
        errorCont "Encountered value out of the range [0, 255]." 1
    elif [ $exit_code -ne 0 ]; then
        # This handles crashes (like file not found, or syntax errors)
        errorCont "Comparison tool (ppmcmp.py) failed." 1
        printf "\033[33m%s\033[0m\n" "$ppmcmp_out"
    # Check numeric difference using awk
    elif awk "BEGIN {exit !($ppmcmp_out <= 5)}"; then
        success "Image verification passed (Diff: $ppmcmp_out)"
    else
        info "Expected output: $output_path"
        info "Actual output: $generated_output_path"
        errorCont "Image mismatch (Diff: $ppmcmp_out)." 1
    fi
    
done

# If check failed
if [ "$doesPass" -eq 0 ]; then
    log ""
    error "Tests finished with errors." 1
fi

# Ending message
log ""; br; success "END: All integration tests passed."; br; echo;