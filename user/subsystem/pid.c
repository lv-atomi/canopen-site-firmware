#include "pid.h"

// Initialize a PID controller
void pid_init(PIDController *pid, double setpoint, double kp, double ki, double kd) {
  pid->setpoint = setpoint;
  pid->kp = kp;
  pid->ki = ki;
  pid->kd = kd;
  pid->prev_error = 0;
  pid->integral = 0;
}

// Update the PID controller and get the control output
double pid_update(PIDController *pid, double current_value) {
  double error = pid->setpoint - current_value;
  pid->integral += error;
  double derivative = error - pid->prev_error;

  double output =
      pid->kp * error + pid->ki * pid->integral + pid->kd * derivative;

  pid->prev_error = error;

  return output;
}

void pid_update_setpoint(PIDController *pid, double new_setpoint, double decay_factor) {
  pid->setpoint = new_setpoint;
  pid->integral *= decay_factor;
  pid->prev_error *= decay_factor;
}


/* int main() { */
/*   PIDController pid; */
/*   pid_init(&pid, 100.0, 1.0, 0.1, 0.01); */

/*   double current_value = 0.0; */
/*   bool running = true; */

/*   while (running) { */
/*     double output = pid_update(&pid, current_value); */

/*     // Here you would send the 'output' to whatever system you're controlling. */
/*     // For this example, we'll just print it out and simulate it driving the */
/*     // 'current_value' closer to the setpoint. */

/*     printf("PID output: %lf\n", output); */

/*     current_value += */
/*         output * 0.05; // Simulate the system response (arbitrarily chosen) */

/*     if (fabs(pid.setpoint - current_value) < 0.01) { */
/*       running = false; // End when close to setpoint */
/*     } */
/*   } */

/*   return 0; */
/* } */
