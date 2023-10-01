#ifndef PID_H
#define PID_H

#include <stdio.h>
#include <stdbool.h>

typedef struct {
  double setpoint;
  double kp;
  double ki;
  double kd;
  double prev_error;
  double integral;
} PIDController;

void pid_init(PIDController *pid, double setpoint, double kp, double ki,
              double kd);

double pid_update(PIDController *pid, double current_value);

#endif
