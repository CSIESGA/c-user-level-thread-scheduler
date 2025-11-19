#!/bin/sh

# fibonacci and stir1 threads
sample_1() {
    ./main 3 6 1 4 -1 -1
}

# fibonacci and stir2 with SIGTSTP signals
sample_2() {
    ./main 3 5 2 3 -1 -1 &
    child=$!
    sleep 1.5
    kill -TSTP $child
    sleep 5
    kill -TSTP $child
    wait $child
}

# worker-coordinator pattern with thread_join and return values
sample_3() {
    ./main 3 -1 -1 -1 10 3
}

# interrupt workers with Ctrl+Z signal
sample_4() {
    ./main 3 -1 -1 -1 10 4 &
    child=$!
    sleep 2.5
    kill -TSTP $child
    sleep 3
    kill -TSTP $child
    sleep 2
    kill -TSTP $child
    wait $child
}

# run all types task with Ctrl+Z signal
sample_5() {
    ./main 3 4 1 4 10 3 &
    child=$!
    sleep 2.5
    kill -TSTP $child
    sleep 3
    kill -TSTP $child
    sleep 2
    kill -TSTP $child
    wait $child
}

print_help() {
    echo "usage: $0 [sample_number]"
    echo ""
    echo "Available samples:"
    echo "  1  - Fibonacci and Stir1"
    echo "  2  - Fibonacci and Stir2 with SIGTSTP signals"
    echo "  3  - Fork-Join pattern with 3 workers and coordinator"
    echo "  4  - Fork-Join with SIGTSTP (Ctrl+Z) interrupts"
    echo "  5  - All tasks with SIGTSTP (Ctrl+Z) interrupts"
    echo ""
    echo "Main usage:"
    echo "  ./main [timeslice] [fib_arg] [stir_type] [stir_n] [sum_n] [sum_k]"
    echo ""
    echo "Note: Use -1 to disable a thread"
}

main() {
    case "$1" in
        1)
            sample_1
            ;;
        2)
            sample_2
            ;;
        3)
            sample_3
            ;;
        4)
            sample_4
            ;;
        5)
            sample_5
            ;;
        *)
            print_help
            ;;
    esac
}

main "$1"
