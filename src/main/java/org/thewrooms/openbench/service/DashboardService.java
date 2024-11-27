package org.thewrooms.openbench.service;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;
import org.thewrooms.openbench.repository.EquipmentRepository;

@Service
public class DashboardService {

    @Autowired
    private EquipmentRepository equipmentRepository;

}
