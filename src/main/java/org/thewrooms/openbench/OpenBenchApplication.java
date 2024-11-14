package org.thewrooms.openbench;

import org.springframework.boot.SpringApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;


@SpringBootApplication
public class OpenBenchApplication {

    public static void main(String[] args) {
        SpringApplication.run(OpenBenchApplication.class, args);
        System.out.println("OpenBench application started");
        System.out.println("Hello World!");
    }

}
