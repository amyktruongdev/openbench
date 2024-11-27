package org.thewrooms.openbench.service;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;
import org.thewrooms.openbench.repository.AuditLogRepository;
import org.thewrooms.openbench.repository.EquipmentRepository;

@Service
public class SensorService {

    @Autowired
    private EquipmentRepository equipmentRepository;

    @Autowired
    private AuditLogRepository auditLogRepository;

}

