package org.thewrooms.openbench.dto;

import lombok.Getter;

public class SensorDto {
    @Getter
    private Long equipmentId;
    private String status;

    public Object getStatus() {
        return status;
    }

    // Getters and setters
}

