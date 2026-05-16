int main() {
    int x = 10;
    int y = 20;

    if (x == y) {
        x = x + 1;
    }
    else if (x != y && x <= y) {
        y = y - 1;
    }

    while (x >= 0) {
        x = x - 1;
    }

    for (int i = 0; i < 5; i = i + 1) {
        y = y * 2 / 3 % 4;
    }

    return x;
}
