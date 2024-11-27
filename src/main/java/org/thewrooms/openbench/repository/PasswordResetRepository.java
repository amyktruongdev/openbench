package org.thewrooms.openbench.repository;

import org.thewrooms.openbench.model.PasswordReset;
import org.springframework.data.jpa.repository.JpaRepository;
import java.util.Optional;

public interface PasswordResetRepository extends JpaRepository<PasswordReset, Integer> {
    Optional<PasswordReset> findByResetTokenHash(String tokenHash);
}

