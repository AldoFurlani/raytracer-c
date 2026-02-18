#!/bin/bash

# *-------------------- Exit Codes --------------------* #
#  0 : Success
#  1 : Runtime/Logic error
#  2 : Configuration error

# Files
required_source_files=("assg.c" "spheres.c" "vector.c")
program_name="vector_tests"

# Compilation and linking flags
compilation_flags="-Wall -Werror -std=c99 -DMS1"
linking_flags="-lm"
time_limit=1 # Increased slightly for safety

# Required directories
source_dir="./src"
bin_dir="./bin"
testcases_dir="./tests/vectors"

# Postfixes
input_postfix="_input.txt"
output_postfix="_output.txt"
generated_postfix="_generated.txt"

# Logging functions
width=$(tput cols);
log()     { printf "\033[00m[UNIT] %s\033[0m\n" "$1"; }
info()    { printf "\033[33m[UNIT] %s\033[0m\n" "$1"; }
success() { printf "\033[32m[UNIT] %s\033[0m\n" "$1"; }
warning() { printf "\033[35m[UNIT] Warning: %s\033[0m\n" "$1"; }
br()      { break=$(printf "%*s" "$((width-8))" "" | tr " " "-"); log "$break"; }
error() {
    printf "\033[31m[UNIT] Error: %s\033[0m\n" "$1";
    log ""
    br; info "END: Suite failed."; br; echo;
    exit "$2";
}

doesPass=1
errorCont() {
    doesPass=0
    printf "\033[31m[UNIT] Fail: %s\033[0m\n" "$1";
}

echo; br; success "STARTING VECTOR UNIT TESTS"; br; log ""

# 1. Environment Checks
if [ ! -d "$testcases_dir" ]; then
    error "Test directory '$testcases_dir' not found." 2
fi
if [ ! -d "$source_dir" ]; then
    error "Source directory '$source_dir' not found." 1
fi

for file in "${required_source_files[@]}"; do
    if [ ! -e "$source_dir/$file" ]; then
        error "Required file '$source_dir/$file' not found." 1
    fi
done

# 2. Build Process
log "Compiling source files..."
if [ -d "$bin_dir" ]; then rm -r "$bin_dir"; fi
mkdir -p "$bin_dir"

for src_file in "$source_dir"/*.c; do
    basename=$(basename "$src_file" .c)
    obj_file="$bin_dir/${basename}.o"
    if ! gcc ${compilation_flags} -c "$src_file" -o "$obj_file"; then
        error "Compilation failed for $src_file" 1
    fi
done

log "Linking $program_name..."
exe_path="./bin/$program_name"
if ! gcc $bin_dir/*.o ${linking_flags} -o "$exe_path"; then
    error "Linking failed." 1
fi
success "Build successful."

# 3. Execution Loop
shopt -s nullglob
for input_path in "$testcases_dir"/*"$input_postfix"; do

    log ""
    test_name=$(basename "$input_path" "$input_postfix")
    output_path=${input_path/"$input_postfix"/"$output_postfix"}
    
    if [[ ! -e $output_path ]]; then
        info "Missing reference for $test_name. Skipping."
        continue
    fi

    generated_output_path=${input_path/"$input_postfix"/"$generated_postfix"}
    log "Running test: $test_name..."
    
    # Run program (direct execution to avoid 'timeout' command issues)
    "$exe_path" "$input_path" "$generated_output_path"
    exit_code=$?

    if [ $exit_code -ne 0 ]; then
        errorCont "Program exited with error code $exit_code."
        continue
    fi

    # 4. Memory Check
    if command -v valgrind &> /dev/null; then
        valgrind_out=$(valgrind --leak-check=full "$exe_path" "$input_path" "$generated_output_path" 2>&1)
        bytes=$(echo "$valgrind_out" | grep "in use at exit" | awk '{print $6}' | tr -d ',')
        bytes=${bytes:-0}
        
        if [ "$bytes" -ne 0 ]; then
            errorCont "Memory leak detected ($bytes bytes)."
        else
            success "Memory integrity: OK"
        fi
    fi

    # 5. Text Comparison (Diff)
    if diff -u "$output_path" "$generated_output_path" > /dev/null; then
        success "Output verification: PASSED"
    else
        errorCont "Output verification: FAILED"
        info "Differences found in $test_name:"
        diff -u "$output_path" "$generated_output_path" | head -n 10
    fi
    
done

# Cleanup build artifacts
rm -rf "$bin_dir"

# Final Status
echo ""
br
if [ "$doesPass" -eq 1 ]; then
    success "END: All vector tests passed."
    # Cleanup generated files only on success
    rm -f "$testcases_dir"/*"$generated_postfix"
    exit 0
else
    error "Suite finished with failures." 1
fi