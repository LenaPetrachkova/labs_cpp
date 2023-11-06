#include <iostream>
#include <thread>
#include <semaphore>
#include <fstream>

using namespace std;


binary_semaphore sem_b(0);
binary_semaphore sem_c(0);
binary_semaphore sem_d(0);
binary_semaphore sem_e(0);
binary_semaphore sem_f(0);
binary_semaphore sem_g(0);
binary_semaphore sem_h(0);
binary_semaphore sem_k(0);
binary_semaphore sem_i(0);
binary_semaphore sem_j(0);

ofstream os("result.txt");

void f(char set, int action) {
    os << "From set " << set << " action " << action << " is performed" << endl;
}

void action_a() {
    for (int i = 1; i <= 7; ++i) {
        f('a', i);
    }
    sem_b.release(); // a -> b
    sem_c.release(); // a -> c
}

void action_b() {
    sem_b.acquire();
    for (int i = 1; i <= 7; ++i) {
        f('b', i);
    }
    sem_d.release(1); // b -> d
    sem_e.release(1); // b -> e
    sem_f.release(1); // b -> f
}

void action_c() {
    sem_c.acquire();
    for (int i = 1; i <= 6; ++i) {
        f('c', i);
    }
    sem_d.release(1); // c -> d
    sem_e.release(1); // c -> e
    sem_f.release(1); // c -> f
}

void action_d() {
    sem_d.acquire();
    sem_d.acquire();
    for (int i = 1; i <= 6; ++i) {
        f('d', i);
    }
    sem_g.release(); // d -> g
    sem_h.release(); // d -> h
    sem_i.release(); // d -> i
    sem_j.release(); // d -> j
}

void action_e() {
    sem_e.acquire();
    sem_e.acquire();
    for (int i = 1; i <= 7; ++i) {
        f('e', i);
    }
    sem_h.release(); // e -> h
}

void action_f() {
    sem_f.acquire();
    sem_f.acquire();
    for (int i = 1; i <= 4; ++i) {
        f('f', i);
    }
    sem_i.release(); // f -> i
}

void action_g() {
    sem_g.acquire();
    for (int i = 1; i <= 4; ++i) {
        f('g', i);
    }
    sem_k.release(); // g -> k
}

void action_h() {
    sem_h.acquire();
    sem_h.acquire();
    for (int i = 1; i <= 7; ++i) {
        f('h', i);
    }
    sem_k.release(); // h -> k
}

void action_i() {
    sem_i.acquire();
    sem_i.acquire();
    for (int i = 1; i <= 4; ++i) {
        f('i', i);
    }
    sem_k.release(); // i -> k
}

void action_j() {
    sem_j.acquire();
    for (int i = 1; i <= 9; ++i) {
        f('j', i);
    }
    sem_k.release(1); // j -> k
}

void action_k() {
    sem_k.acquire();
    sem_k.acquire();
    sem_k.acquire();
    sem_k.acquire();
    for (int i = 1; i <= 5; ++i) {
        f('k', i);
    }
}

int main() {

    os << "Calculations started" << endl;

    thread ta(action_a);
    thread tb(action_b);
    thread tc(action_c);
    thread td(action_d);
    thread te(action_e);
    thread tf(action_f);
    thread tg(action_g);
    thread th(action_h);
    thread ti(action_i);
    thread tj(action_j);
    thread tk(action_k);

    ta.join();
    tb.join();
    tc.join();
    td.join();
    te.join();
    tf.join();
    tg.join();
    th.join();
    ti.join();
    tj.join();
    tk.join();

    os << "Calculations finished" << endl;
    os.close();

    return 0;
}
