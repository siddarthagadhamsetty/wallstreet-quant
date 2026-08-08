#include <stdio.h>
#include <math.h>

#define N 10
#define WINDOW 3
#define THETA 0.75

double rolling_mean(double prices[], int end) {
    double sum = 0.0;
    for (int i = end - WINDOW + 1; i <= end; i++) {
        sum += prices[i];
    }
    return sum / WINDOW;
}

double rolling_sd(double prices[], int end) {
    double mean = rolling_mean(prices, end);
    double sum_sq = 0.0;

    for (int i = end - WINDOW + 1; i <= end; i++) {
        double diff = prices[i] - mean;
        sum_sq += diff * diff;
    }

    return sqrt(sum_sq / (WINDOW - 1));   // sample standard deviation
}

int position_from_z(double z) {
    if (z < -THETA) return 1;   // long
    if (z > THETA)  return -1;  // short
    return 0;                  // flat
}

void backtest(char *name, double prices[]) {
    printf("\n=== %s ===\n", name);
    printf("Day\tPrice\tMean\t\tSD\t\tZ\t\tPos\tNextRet\t\tStratRet\n");

    double cumulative = 1.0;

    for (int t = WINDOW - 1; t < N - 1; t++) {
        double mean = rolling_mean(prices, t);
        double sd   = rolling_sd(prices, t);
        double z    = (prices[t] - mean) / sd;
        int pos     = position_from_z(z);

        double next_ret = (prices[t + 1] - prices[t]) / prices[t];
        double strat_ret = pos * next_ret;

        cumulative *= (1.0 + strat_ret);

        printf("%d\t%.2f\t%.4f\t\t%.4f\t\t%.4f\t\t%d\t%.4f\t\t%.4f\n",
               t + 1, prices[t], mean, sd, z, pos, next_ret, strat_ret);
    }

    printf("Cumulative return: %.4f%%\n", (cumulative - 1.0) * 100.0);
}

int main() {
    double indexETF[N]  = {250, 248, 252, 249, 253, 251, 247, 250, 254, 252};
    double sectorETF[N] = {180, 183, 179, 182, 178, 181, 184, 180, 177, 181};

    backtest("Index ETF", indexETF);
    backtest("Sector ETF", sectorETF);

    return 0;
}
