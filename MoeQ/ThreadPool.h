/**
 * Tencent is pleased to support the open source community by making Tars available.
 *
 * Copyright (C) 2016THL A29 Limited, a Tencent company. All rights reserved.
 *
 * Licensed under the BSD 3-Clause License (the "License"); you may not use this file except
 * in compliance with the License. You may obtain a copy of the License at
 *
 * https://opensource.org/licenses/BSD-3-Clause
 *
 * Unless required by applicable law or agreed to in writing, software distributed
 * under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
 * CONDITIONS OF ANY KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations under the License.
 */


#pragma once
/**
 * @file tc_thread_pool.h
 * @brief �̳߳���,����c++11��ʵ����
 * ʹ��˵��:
 * ThreadPool tpool;
 * tpool.init(5);   //��ʼ���̳߳��߳���
 * //�����߳������ַ�ʽ
 * //��һ��, ֱ������
 * tpool.start();
 * //�ڶ���, ����ʱָ����ʼ������, ���綨�庯��
 * void testFunction(int i)
 * {
 *     cout << i << endl;
 * }
 * tpool.start(testFunction, 5);    //start�ĵ�һ������std::bind���صĺ���(std::function), ���������
 * //�����񶪵��̳߳���
 * tpool.exec(testFunction, 10);    //������start��ͬ
 * //�ȴ��̳߳ؽ���, �����ַ�ʽ:
 * //��һ�ֵȴ��̳߳���������
 * tpool.waitForAllDone(1000);      //����<0ʱ, ��ʾ���޵ȴ�(ע�����˵���stopҲ���Ƴ�)
 * //�ڶ��ֵȴ��ⲿ���˵����̳߳ص�stop����
 * tpool.waitForStop(1000);
 * //��ʱ: �ⲿ��Ҫ�����̳߳��ǵ���
 * tpool.stop();
 * ע��:
 * ThreadPool::execִ�����񷵻ص��Ǹ�future, ��˿���ͨ��future�첽��ȡ���, ����:
 * int testInt(int i)
 * {
 *     return i;
 * }
 * auto f = tpool.exec(testInt, 5);
 * cout << f.get() << endl;   //��testInt���̳߳���ִ�к�, f.get()�᷵����ֵ5
 *
 * class Test
 * {
 * public:
 *     int test(int i);
 * };
 * Test t;
 * auto f = tpool.exec(std::bind(&Test::test, &t, std::placeholders::_1), 10);
 * //���ص�future����, ���Լ���Ƿ�ִ��
 * cout << f.get() << endl;
 * @author  jarodruan@upchina.com
 */

#include <future>
#include <functional>
#include <queue>
#include <mutex>
#include <thread>

/**
 * @brief ��ͨ�̳߳���(����c++11ʵ��)
 *
 * ʹ�÷�ʽ˵��:
 * ����ʾ��������μ�:examples/util/example_tc_thread_pool.cpp
 */
class ThreadPool
{
public:

    /**
     * @brief ����, ��ֹͣ�����߳�
     */
    ~ThreadPool();

    /**
     * @brief ��ʼ��.
     *
     * @param num �����̸߳���
     */
    void init(size_t num);

    /**
     * @brief ��ȡ�̸߳���.
     *
     * @return size_t �̸߳���
     */
    size_t getThreadNum()
    {
        std::unique_lock<std::mutex> lock(_mutex);

        return _threads.size();
    }

    /**
     * @brief ��ȡ��ǰ�̳߳ص�������
     *
     * @return size_t �̳߳ص�������
     */
    size_t getJobNum()
    {
        std::unique_lock<std::mutex> lock(_mutex);
        return _tasks.size();
    }

    /**
     * @brief ֹͣ�����߳�, ��ȴ������߳̽���
     */
    void stop();

    /**
     * @brief ���������߳�
     */
    void start();

    /**
     * @brief ���̳߳���������(F��function, Args�ǲ���)
     *
     * @param ParentFunctor
     * @param tf
     * @return ���������future����, ����ͨ�������������ȡ����ֵ
     */
    template <class F, class... Args>
    auto exec(F&& f, Args&&... args) -> std::future<decltype(f(args...))>
    {
        //���巵��ֵ����
        using RetType = decltype(f(args...));

        auto task = std::make_shared<std::packaged_task<RetType()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));

        std::future<RetType> res = task->get_future();

        std::unique_lock<std::mutex> lock(_mutex);

        _tasks.emplace([task]() { (*task)(); });

        _condition.notify_one();

        return res;

    }

    /**
     * @brief �ȴ���ǰ���������, ���й���ȫ������(����������).
     *
     * @param millsecond �ȴ���ʱ��(ms), -1:��Զ�ȴ�
     * @return           true, ���й������������
     *                   false,��ʱ�˳�
     */
    bool waitForAllDone(int millsecond = -1);

protected:
    /**
     * @brief ��ȡ����
     *
     * @return std::function<void()>
     */
    bool get(std::function<void()>& task);

    /**
     * @brief �̳߳��Ƿ��˳�
     */
    bool isTerminate() { return _bTerminate; }

    /**
     * @brief �߳�����̬
     */
    void run();

protected:

    /**
     * �������
     */
    std::queue<std::function<void()>> _tasks;

    /**
     * �����߳�
     */
    std::vector<std::thread*> _threads;

    std::mutex                _mutex;

    std::condition_variable   _condition;

    size_t                    _threadNum;

    bool                      _bTerminate;

    std::atomic<int>          _atomic{ 0 };
};