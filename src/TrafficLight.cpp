#include <iostream>
#include <random>
#include "TrafficLight.h"
#include <chrono>

/* Implementation of class "MessageQueue" */


template <typename T>
T MessageQueue<T>::receive()
{
    // FP.5a : The method receive should use std::unique_lock<std::mutex> and _condition.wait() 
    // to wait for and receive new messages and pull them from the queue using move semantics. 
    // The received object should then be returned by the receive function. 

    std::unique_lock<std::mutex> lock_mechnism(_mutex);
    _condition.wait(lock_mechnism);
    T msg = std::move(_queue.back());
    _queue.pop_back();
    return msg;
}

template <typename T>
void MessageQueue<T>::send(T &&msg)
{
    // FP.4a : The method send should use the mechanisms std::lock_guard<std::mutex> 
    // as well as _condition.notify_one() to add a new message to the queue and afterwards send a notification.

  
    std::lock_guard<std::mutex> lock_mechanism(_mutex);

    _queue.push_back(std::move(msg));
    //_queue.emplace_back(std::move(msg));
    //_queue.push_back(msg);
    _condition.notify_one(); 

}


/* Implementation of class "TrafficLight" */

 
TrafficLight::TrafficLight()
{
    _currentPhase = TrafficLightPhase::red;
}

void TrafficLight::waitForGreen()
{
    // FP.5b : add the implementation of the method waitForGreen, in which an infinite while-loop 
    // runs and repeatedly calls the receive function on the message queue. 
    // Once it receives TrafficLightPhase::green, the method returns.
    while(true){
        
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        if(_queue_member.receive() == TrafficLightPhase::green)
        return;
    }


}

TrafficLightPhase TrafficLight::getCurrentPhase()
{
    return _currentPhase;
}


void TrafficLight::simulate()
{
    // FP.2b : Finally, the private method „cycleThroughPhases“ should be started in a thread when the public method „simulate“ is called. To do this, use the thread queue in the base class. 
    threads.emplace_back(std::thread(&TrafficLight::cycleThroughPhases, this));
}

// virtual function which is executed in a thread


// https://stackoverflow.com/questions/7560114/random-number-c-in-some-range

void TrafficLight::cycleThroughPhases()
{
    // FP.2a : Implement the function with an infinite loop that measures the time between two loop cycles 
    // and toggles the current phase of the traffic light between red and green and sends an update method 
    // to the message queue using move semantics. The cycle duration should be a random value between 4 and 6 seconds. 
    // Also, the while-loop should use std::this_thread::sleep_for to wait 1ms between two cycles.

 
    auto previous_time = std::chrono::system_clock::now();

    std::random_device rd;
    std::mt19937 eng(rd());
    std:: uniform_int_distribution<> distr(4*100,6*100); //in milisec

    


    while (true) {

        //auto current_time = std::chrono::system_clock::now();

        long loop_duration = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - previous_time).count();

        //long loop_duration = std::chrono::duration_cast<std::chrono::seconds> (current_time - previous_time);
        //auto loop_duration =(current_time - previous_time);

        auto light_cycle_duration = distr(eng)/100.0; //static_cast <float>(distr(eng)/100.0);

        std::this_thread::sleep_for(std::chrono::milliseconds(1));

    

        //if(loop_duration > light_cycle_duration){
        if(loop_duration >= light_cycle_duration){    
            if (_currentPhase == red)
                _currentPhase = green;
            if (_currentPhase == green)
                _currentPhase = red;

            auto light_state = _currentPhase;
           // auto update_state = std::async(std::launch::async, &MessageQueue<TrafficLightPhase>::send, _msgQueue, std::move(light_state)); //fix me
            //update_state.wait(); // wait for resuls
            //light_cycle_duration = static_cast <float>(distr(eng)/100.0);
            previous_time = std::chrono::high_resolution_clock::now();
        
        }

    } 
}

