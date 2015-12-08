 
/*
 * This launches the whole SESC simulator environment
 */

#include <stdlib.h>
#include <vector>

#include "nanassert.h"

#include "SMTProcessor.h"
#include "Processor.h"

#include "MemorySystem.h"

#include "OSSim.h"
#include "SescConf.h"

#ifdef TASKSCALAR
#include "TaskHandler.h"
#endif

int32_t main(int32_t argc, char **argv, char **envp)
{ 
#ifdef TASKSCALAR
  taskHandler = new TaskHandler();
#endif
  osSim = new OSSim(argc, argv, envp);  //初始化一个OS模拟器?

  int32_t nProcs = SescConf->getRecordSize("","cpucore");  //得到处理器的个数

  std::vector<GMemorySystem *> ms(nProcs);                 //nProcs个内存子系统
  std::vector<GProcessor *>    pr(nProcs);                 //nProcs个处理器

  for(int32_t i = 0; i < nProcs; i ++) {                   //初始化内存子系统和处理器
    GMemorySystem *gms = new MemorySystem(i);              //内存子系统
    gms->buildMemorySystem();                              //建一个内存OS,调用MemorySystem 中的build建立
    ms[i] = gms;
    pr[i] = 0;
	
	/*建一个处理器i，这个处理器和ms[i] 挂钩//这个处理器也会在osSim中注册*/
    if(SescConf->checkInt("cpucore","smtContexts",i)) {
      if( SescConf->getInt("cpucore","smtContexts",i) > 1 )
	pr[i] =new SMTProcessor(ms[i], i);
    }
    if (pr[i] == 0)
      pr[i] =new Processor(ms[i], i);  //建一个处理器，这个处理器会在RunningProcs cpus 中进行注册
  }

  osSim->boot();

  // Reaches this point only when all the active threads have finished.
  
  
  for(int32_t i = 0; i < nProcs; i ++) {
    GProcessor *gp = pr[i];
    delete ms[i];
  }

  delete osSim;
  

  return 0;
}
