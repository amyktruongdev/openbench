package org.thewrooms.openbench.model;

import jakarta.persistence.*;
import lombok.Setter;

import java.time.LocalDateTime;

@Entity
@Table(name = "equipment")
public class Equipment {
    @Id
    @GeneratedValue(strategy = GenerationType.IDENTITY)
    private Long id;

    private String type;
    private String status; // e.g., "Occupied" or "Available"
    private String location;
    private int powerLevel;
    @Setter
    private LocalDateTime lastUsed;

}

