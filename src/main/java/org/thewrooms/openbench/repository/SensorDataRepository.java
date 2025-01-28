package org.thewrooms.openbench.repository;

import org.springframework.data.jpa.repository.JpaRepository;
import org.thewrooms.openbench.model.SensorData;

public interface SensorDataRepository extends JpaRepository<SensorData, Long> {
}

