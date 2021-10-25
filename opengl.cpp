internal void
display_buffer_in_window(render_state *rs)
{
    //Sleep(1000);
    locked_exchange_u32(&rs->context_ready, 1);
    
    LARGE_INTEGER timer_start;
    LARGE_INTEGER frequency;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&timer_start);
    
    while(rs->queue->tiles_retired < rs->total_tile_count)
    {
        printf("\rrendering... %.0f%%", ((f32)rs->queue->tiles_retired / (f32)rs->queue->work_order_count) * 100);
        fflush(stdout);
    }
    
    printf("\rrendering... %.0f%%", ((f32)rs->queue->tiles_retired / (f32)rs->queue->work_order_count) * 100);
    fflush(stdout);
    printf("\n");
    
    LARGE_INTEGER timer_end;
    QueryPerformanceCounter(&timer_end);
    timer_end.QuadPart = timer_end.QuadPart - timer_start.QuadPart;
    timer_end.QuadPart *= 1000000;
    timer_end.QuadPart /= frequency.QuadPart;
    f64 ms_elapsed = (f64)timer_end.QuadPart / (f64)1000;
    f64 ms_per_bounce = ms_elapsed / rs->queue->bounces_computed;
    printf("runtime: %.3Lf ms\n", ms_elapsed);
    printf("per-ray performance: %Lf ms\n", ms_per_bounce);
    locked_exchange_u32(&rs->render_in_progress, 0);
}

internal DWORD 
WINAPI render_thread(void *lpParameter)
{
    render_state *rs = (render_state *)lpParameter;
    display_buffer_in_window(rs);
    return 0;
}

internal void
begin_render_thread(void *param)
{
    DWORD thread_id;
    HANDLE thread_handle = CreateThread(0, 0, render_thread, param, 0, &thread_id);
    CloseHandle(thread_handle);
}
