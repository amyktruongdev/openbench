package org.thewrooms.openbench.repository;

import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.data.jpa.repository.Query;
import org.thewrooms.openbench.model.Equipment;

public interface EquipmentRepository extends JpaRepository<Equipment, Long> {

    @Query("SELECT COUNT(e) FROM Equipment e WHERE e.status = 'Occupied'")
    int countOccupiedEquipment();

    @Query("SELECT COUNT(e) FROM Equipment e WHERE e.type = :type AND e.status = :status")
    int countByTypeAndStatus(String type, String status);
}

