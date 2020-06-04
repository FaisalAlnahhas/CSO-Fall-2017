SYSCALL_DEFINE2(procstat, int pid, struct proc_stat* user){

  struct proc_stat temp;
  task_struct* p;
  
  #if p is null do eserch
  

  if (0 < pid < 32768){
    p = get_task_by_pid_ns(pid, &init_pid_ns); 
    if (p == NULL){
      return ESRCH;
    }
    temp.parent_pid = p -> real_parent -> pid;
    temp.pid = p -> pid;
    temp.rt_priority = p -> rt_priority;
    temp.user_time = p -> utime;
    temp.sys_time = p -> stime;
    temp.state = p -> state;
    temp.priority = p -> prio; 
    temp.normal_priority = p -> normal_prio;
    temp.static_priority = p -> static_prio;
    temp.time_slice = p -> rt.time_slice;
    temp.policy = p -> policy;
    temp.num_context_switches = p -> num_context_switches;
    temp.task_size = p -> mm->task_size;
    temp.total_pages_mapped = p -> mm -> total_pages_mapped;
    memcpy( temp.name, ts -> comm, 16);
    
  }
  
  else {
    return EINVAL;
  }
  
  int ret = copy_to_user(user, &temp, sizeof(struct proc_stat));
  
  if (ret < 0) {
    return EFAULT;
  }
  
  return 0;
  
 
  
 
  
}


