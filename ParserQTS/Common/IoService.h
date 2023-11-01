//////////////////////////////////////////////////////////////////////////////
/// @file       IoService.h                                                      
/// @brief      ���������                     
/// @version    v1.0.0                                                        
/// @note                                                                         
//////////////////////////////////////////////////////////////////////////////

#ifndef MAIN_THREAD_H
#define MAIN_THREAD_H

#include <memory>
#include <functional>
#include <deque>
#include "TinyThread.h"

typedef std::function<void (void)> Task;
typedef std::unique_ptr<Task> TaskPtr;

/**����˳��
   1.����Start()�ӿڣ�
   2.ִ������ʱ��ʹ��Post()�ӿڣ�
   3.����Stop()�ӿڣ�
*/

/// ���������
class IoService
{
public:
    /// ����
    IoService();
    /// ����
    virtual ~IoService();

    /// ����
    void Start();

    /// ����
    void Run();
    /// ֹͣ
    void Stop();
    /// Ͷ������
    /// @param task ����
    void Post(TaskPtr& task);

private:
    static void IosProc(void* arg);

private:
    tthread::mutex              m_mutex; ///< ��
    tthread::condition_variable m_cv;    ///< ��������
    bool                        m_stop;  ///< ֹͣ��־
    std::deque<TaskPtr>         m_tasks; ///< �����б�

    std::unique_ptr<tthread::thread>	m_iosThread;  ///< ִ�������߳�
};

#endif // MAIN_THREAD_H