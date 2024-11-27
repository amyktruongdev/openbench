package org.thewrooms.openbench.repository;

import org.springframework.data.jpa.repository.JpaRepository;
import org.thewrooms.openbench.model.AuditLog;

public interface AuditLogRepository extends JpaRepository<AuditLog, Integer> {
}
