echo "Running basic commands test..."
ls
pwd
history
echo "This is a test message."
echo "Basic commands test complete."
echo "Starting background job test..."
sleep 10 &
echo "Background job started with PID: $bg_pid"
sleep 2
wait $bg_pid
echo "Background job test complete."