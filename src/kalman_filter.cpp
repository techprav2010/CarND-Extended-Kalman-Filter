#include "kalman_filter.h"
#include <math.h>

using Eigen::MatrixXd;
using Eigen::VectorXd;

KalmanFilter::KalmanFilter() {}

KalmanFilter::~KalmanFilter() {}

void KalmanFilter::Init(VectorXd &x_in, MatrixXd &P_in, MatrixXd &F_in,
                        MatrixXd &H_in, MatrixXd &R_in, MatrixXd &Q_in) {
    x_ = x_in;
    P_ = P_in;
    F_ = F_in;
    H_ = H_in;
    R_ = R_in;
    Q_ = Q_in;
}

void KalmanFilter::Predict() {
    /**
    TODO:
      * predict the state
    */
    x_ = F_ * x_;
    MatrixXd Ft = F_.transpose();
    P_ = F_ * P_ * Ft + Q_;
}

void KalmanFilter::UpdateEKF(const VectorXd &z, bool useExtended) {
    /**
    TODO:
      * update the state by using Extended Kalman Filter equations
    */
    VectorXd z_pred;
    if (useExtended) {
        double r, r_dot, th, px, py, vx, vy;
        px = x_(0);
        py = x_(1);
        vx = x_(2);
        vy = x_(3);
        r = sqrt(px * px + py * py);
        th = atan2(py, px);
        if (fabs(r) < 0.0001) {
            r = 0;
        }
        r_dot = (px * vx + py * vy) / r;
        z_pred = VectorXd(3);
        z_pred << r, th, r_dot;
    } else {
        z_pred = H_ * x_;
    }

    VectorXd y = z - z_pred;
    // Normalize the angle between pi and -pi
    while (y(1) > M_PI) {
        y(1) -= 2 * M_PI;
    }
    while (y(1) < -M_PI) {
        y(1) += 2 * M_PI;
    }
    MatrixXd Ht = H_.transpose();
    MatrixXd PHt = P_ * Ht;
    MatrixXd S = H_ * PHt + R_;
    MatrixXd Si = S.inverse();
    MatrixXd K = PHt * Si;

    //New estimate
    x_ = x_ + (K * y);
    long x_size = x_.size();
    MatrixXd I = MatrixXd::Identity(x_size, x_size);
    P_ = (I - K * H_) * P_;
}
