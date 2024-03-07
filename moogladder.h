/*
C++ Implementation of the Moog Ladder Filter by Daan & Mila of Triceratops
based on: "Oscillators and Filter Algorithms for Virtual Analog Synthesis"
by Vesa Välimäki and Antti Huovilainen
published in 2006

Special thanks to Wouter Ensink
Special thanks to our lovely teachers: Pieter Suurmond, Ciska Vriezenga, Marc Groenewegen
*/

#pragma once

#include <array>
#include <cmath>
#include <numbers>

struct LadderCoefficients {
    double A, B, C, D, E;
};

struct MoogLadder {
    MoogLadder() {}
    MoogLadder (double sampleRate) : Fs (sampleRate) {}

    void prepare (double sampleRate) {
        Fs = sampleRate;
    }

    double process (double input) {
        const auto [A, B, C, D, E] = currentCoefficients;

        const auto feed = 4.0 * currentResonance * (pathE - (tanh (input * 0.5)));
        const auto pathA = input - feed;
        const auto pathB = components[0].process (pathA);
        const auto pathC = components[1].process (pathB);
        const auto pathD = components[2].process (pathC);
        pathE = components[3].process (pathD);
        return (pathA * A) + (pathB * B) + (pathC * C) + (pathD * D) + (pathE * E);
    }

    void setCoefficients (const LadderCoefficients& coefficients) noexcept {
        currentCoefficients = coefficients;
    }

    void setCriticalFrequency (double frequency) {
        calculateOmega (frequency);
        for (auto& component : components)
            component.calculateG (omega);
    }

    void setResonance (double resonance) {
        res = resonance;
        currentResonance = resonance * (1.0029f + (0.0526f * omega) - (0.0926f * pow (omega, 2.0f)) + (0.0218f * pow (omega, 3.0f)));
    }


    static LadderCoefficients makeLoPass12() noexcept {
        return {
            .A = 0,
            .B = 0,
            .C = 1,
            .D = 0,
            .E = 0
        };
    }

    static LadderCoefficients makeLoPass24() noexcept {
        return {
            .A = 0,
            .B = 0,
            .C = 0,
            .D = 0,
            .E = 1
        };
    }

    static LadderCoefficients makeBandPass12() noexcept {
        return {
            .A = 0,
            .B = 2,
            .C = -2,
            .D = 0,
            .E = 0
        };
    }

    static LadderCoefficients makeBandPass24() noexcept {
        return {
            .A = 0,
            .B = 0,
            .C = 4,
            .D = -8,
            .E = 4
        };
    }

    static LadderCoefficients makeHiPass12() noexcept {
        return {
            .A = 1,
            .B = -2,
            .C = 1,
            .D = 0,
            .E = 0
        };
    }

    static LadderCoefficients makeHiPass24() noexcept {
        return {
            .A = 1,
            .B = -4,
            .C = 6,
            .D = -4,
            .E = 1
        };
    }


private:
    struct MoogComponent {
        double process (double input) {
            return feedback (feedforward (input));
        }

        void calculateG (double omega) noexcept {
            g = (0.9892 * omega) - (0.4342 * std::pow (omega, 2.0)) + (0.1381 * std::pow (omega, 3.0)) - (0.0202 * std::pow (omega, 4.0));
        }

    private:
        double feedforward (double input) noexcept {
            auto zero = a * input + b * x1;

            x1 = input;
            return zero;
        }

        double feedback (double input) noexcept {
            y1 = g * (input - y1) + y1;
            return y1;
        }

        double a = 1.0 / 1.3;
        double b = 0.3 / 1.3;

        double y1 { 0.0 };
        double x1 { 0.0 };
        double g { 0.0 };
    };

    void calculateOmega (double criticalFrequency) {
        omega = 2 * Pi * criticalFrequency / Fs;
        setResonance(res);
    }

    LadderCoefficients currentCoefficients;
    std::array<MoogComponent, 4> components;
    double Pi = std::numbers::pi_v<double>;
    double Fs { 44100.0 };
    double omega { 0.0 };

    double currentResonance { 0.0 };
    double res { 0.0 };
    double pathE { 0.0 };
};